//
// Created by pqpo on 2017/11/16.
//

#include "includes/LogBuffer.h"

LogBuffer::LogBuffer(char *ptr, size_t buffer_size):
        buffer_ptr(ptr),
        buffer_size(buffer_size),
        logHeader(buffer_ptr, buffer_size) {
    if (logHeader.isAvailable()) {
        data_ptr = (char *) logHeader.ptr();
        write_ptr = (char *) logHeader.write_ptr();
    }
}

LogBuffer::~LogBuffer() {
    release();
}

size_t LogBuffer::length() {
    return write_ptr - data_ptr;
}

void LogBuffer::setLength(size_t len) {
    logHeader.setLogLen(len);
}

size_t LogBuffer::append(const char *log, size_t len) {
    std::lock_guard<std::recursive_mutex> lck_append(log_mtx);
    size_t freeSize = emptySize();
    size_t writeSize = len <= freeSize ? len : freeSize;
    memcpy(write_ptr, log, writeSize);
    write_ptr += writeSize;
    setLength(length());
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
    setLength(length());
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

void LogBuffer::initData(char *log_path, size_t log_path_len) {
    std::lock_guard<std::recursive_mutex> lck_release(log_mtx);
    memset(buffer_ptr, '\0', buffer_size);

    log_header::Header header;
    header.magic = kMagicHeader;
    header.log_path_len = log_path_len;
    header.log_path = log_path;
    header.log_len = 0;
    logHeader.initHeader(header);

    data_ptr = (char *) logHeader.ptr();
    write_ptr = (char *) logHeader.write_ptr();
}

char *LogBuffer::getLogPath() {
    return logHeader.getLogPath();
}

void LogBuffer::flushToBuffer(FlushBuffer *flushBuffer) {
    flushBuffer->write(data_ptr, length());
    clear();
}






