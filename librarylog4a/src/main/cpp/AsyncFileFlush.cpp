//
// Created by pqpo on 2017/11/23.
//

#include "AsyncFileFlush.h"

AsyncFileFlush::AsyncFileFlush(int log_fd):log_fd(log_fd) {
    async_thread = std::thread(&AsyncFileFlush::async_log_thread, this);
}

AsyncFileFlush::~AsyncFileFlush() {
    stopFlush();
}

void AsyncFileFlush::async_log_thread() {
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

ssize_t AsyncFileFlush::flush(char *data) {
    ssize_t written = 0;
    size_t str_len = strlen(data);
    if(log_fd != -1 && str_len > 0) {
        written = write(log_fd, data, str_len);
    }
    return written;
}

bool AsyncFileFlush::async_flush(char *data) {
    std::unique_lock<std::mutex> lck_async_flush(async_mtx);
    if (exit) {
        return false;
    }
    async_buffer.push_back(data);
    async_condition.notify_all();
    return true;
}

void AsyncFileFlush::stopFlush() {
    exit = true;
    async_condition.notify_all();
    async_thread.join();
}
