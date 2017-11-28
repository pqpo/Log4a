//
// Created by pqpo on 2017/11/16.
//

#include "includes/LogBuffer.h"

LogBuffer::LogBuffer(char *ptr, size_t capacity):
        buffer_ptr(ptr),
        buffer_capacity(capacity) {
    offset = strlen(ptr);
}

LogBuffer::~LogBuffer() {
    release();
}

char *LogBuffer::ptr() {
    return buffer_ptr;
}

size_t LogBuffer::capacity() {
    return buffer_capacity;
}

size_t LogBuffer::size() {
    return offset;
}

char *LogBuffer::dataCopy() {
    size_t str_len = size() + 1;  //'\0'
    char* data = new char[str_len];
    memcpy(data, ptr(), str_len);
    data[str_len - 1] = '\0';
    return data;
}

size_t LogBuffer::append(const char *log) {
    std::lock_guard<std::recursive_mutex> lck_append(log_mtx);
    size_t len = strlen(log);
    size_t freeSize = emptySize();
    size_t writeSize = len <= freeSize ? len : freeSize;
    memcpy(ptr() + offset, log, writeSize);
    offset += writeSize;
    return writeSize;
}

void LogBuffer::async_flush(AsyncFileFlush *fileFlush) {
    std::lock_guard<std::recursive_mutex> lck_clear(log_mtx);
    char *data = dataCopy();
    if(strlen(data) == 0 || !fileFlush->async_flush(data)) {
        delete[] data;
    } else {
        clear();
    }
}

void LogBuffer::clear() {
    std::lock_guard<std::recursive_mutex> lck_clear(log_mtx);
    memset(ptr(), '\0', capacity());
    offset = 0;
}

void LogBuffer::release() {
    std::lock_guard<std::recursive_mutex> lck_release(log_mtx);
    if(map_buffer) {
        munmap(ptr(), capacity());
    } else {
        delete[] ptr();
    }
}

size_t LogBuffer::emptySize() {
    return capacity() - size() - 1;
}






