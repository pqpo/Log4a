//
// Created by pqpo on 2017/11/16.
//

#include "includes/LogBuffer.h"

LogBuffer::LogBuffer(char *ptr, size_t buffer_size):
        buffer_ptr(ptr),
        buffer_size(buffer_size) {
    //todo add header data len
    data_ptr = buffer_ptr + 1 + buffer_ptr[0];
    write_ptr = data_ptr + strlen(data_ptr);
}

LogBuffer::~LogBuffer() {
    release();
}

size_t LogBuffer::length() {
    return write_ptr - data_ptr;
}

size_t LogBuffer::append(const char *log, size_t len) {
    std::lock_guard<std::recursive_mutex> lck_append(log_mtx);
    size_t freeSize = emptySize();
    size_t writeSize = len <= freeSize ? len : freeSize;
    memcpy(write_ptr, log, writeSize);
    write_ptr += writeSize;
    return writeSize;
}

void LogBuffer::async_flush(AsyncFileFlush *fileFlush) {
    std::lock_guard<std::recursive_mutex> lck_clear(log_mtx);
    if (length() > 0) {
        FlushBuffer* flushBuffer = new FlushBuffer();
        flushToBuffer(flushBuffer);
        fileFlush->async_flush(flushBuffer);
    }
}

void LogBuffer::clear() {
    std::lock_guard<std::recursive_mutex> lck_clear(log_mtx);
    write_ptr = data_ptr;
    memset(write_ptr, '\0', emptySize());
}

void LogBuffer::release() {
    std::lock_guard<std::recursive_mutex> lck_release(log_mtx);
    if(map_buffer) {
        munmap(buffer_ptr, buffer_size);
    } else {
        delete[] buffer_ptr;
    }
}

size_t LogBuffer::emptySize() {
    return buffer_size - (write_ptr - buffer_ptr);
}

void LogBuffer::initData(const char *log_path) {
    std::lock_guard<std::recursive_mutex> lck_release(log_mtx);
    memset(buffer_ptr, '\0', buffer_size);
    size_t log_path_len = strlen(log_path);
    buffer_ptr[0] = static_cast<char>(log_path_len);
    memcpy(buffer_ptr + 1, log_path, log_path_len);
    data_ptr = buffer_ptr + 1 + log_path_len;
    write_ptr = data_ptr;
}

char *LogBuffer::getLogPath() {
    size_t path_len = static_cast<size_t >(buffer_ptr[0]);
    char* file_path = nullptr;
    if(path_len > 0) {
        file_path = new char[path_len + 1];
        memcpy(file_path, buffer_ptr + 1, path_len);
        file_path[path_len] = '\0';
    }
    return file_path;
}

void LogBuffer::flushToBuffer(FlushBuffer *flushBuffer) {
    flushBuffer->write(data_ptr, length());
    clear();
}






