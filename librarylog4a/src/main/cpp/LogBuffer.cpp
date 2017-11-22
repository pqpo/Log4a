//
// Created by pqpo on 2017/11/16.
//

#include "includes/LogBuffer.h"

LogBuffer::LogBuffer(char *ptr, size_t capacity, int log_fd):
        buffer_ptr(ptr),
        buffer_capacity(capacity),
        log_file_fd(log_fd) {
    async_thread = std::thread(&LogBuffer::async_log_thread, this);
}

LogBuffer::~LogBuffer() {
    exit = true;
    async_condition.notify_all();
    async_thread.join();
    release();
}

char *LogBuffer::ptr() {
    return buffer_ptr;
}

size_t LogBuffer::capacity() {
    return buffer_capacity;
}

size_t LogBuffer::size() {
    return strlen(ptr());
}

char *LogBuffer::dataCopy() {
    size_t str_len = size() + 1;  //'\0'
    char* data = new char[str_len];
    memcpy(data, ptr(), str_len);
    data[str_len - 1] = '\0';
    return data;
}

size_t LogBuffer::append(const char *log) {
    std::unique_lock<std::mutex> lck_append(log_mtx);
    size_t len = strlen(log);
    size_t freeSize = emptySize();
    size_t writeSize = len <= freeSize ? len : freeSize;
    memcpy(ptr() + size(), log, writeSize);
    return writeSize;
}

ssize_t LogBuffer::flush() {
    char* data = dataCopy();
    ssize_t written = flush(data);;
    delete[] data;
    return written;
}

ssize_t LogBuffer::flush(char* data) {
    return flush(data, strlen(data));
}

ssize_t LogBuffer::flush(char* data, size_t str_len) {
    ssize_t written = 0;
    if(log_file_fd != -1 && str_len > 0) {
        written = write(log_file_fd, data, str_len);
    }
    return written;
}

void LogBuffer::async_flush() {
    std::unique_lock<std::mutex> lck_async_flush(async_mtx);
    if(size() > 0) {
        async_buffer.push_back(dataCopy());
        async_condition.notify_all();
        clear();
    }
}

void LogBuffer::clear() {
    std::unique_lock<std::mutex> lck_clear(log_mtx);
    memset(ptr(), '\0', capacity());
}

void LogBuffer::release() {
    std::unique_lock<std::mutex> lck_release(log_mtx);
    if(map_buffer) {
        munmap(ptr(), capacity());
    } else {
        delete[] ptr();
    }
}

size_t LogBuffer::emptySize() {
    return capacity() - size() - 1;
}

void LogBuffer::async_log_thread() {
    while (true) {
        std::unique_lock<std::mutex> lck_async_log_thread(async_mtx);
        while (!async_buffer.empty()) {
            char* data = async_buffer.back();
            flush(data);
            async_buffer.pop_back();
            delete[] data;
        }
        if (exit) {
            return;
        }
        async_condition.wait(lck_async_log_thread);
    }
}






