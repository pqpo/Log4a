//
// Created by pqpo on 2017/11/23.
//

#include "includes/AsyncFileFlush.h"

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
            FlushBuffer* data = async_buffer.back();
            async_buffer.pop_back();
            flush(data);
        }
        if (exit) {
            return;
        }
        async_condition.wait(lck_async_log_thread);
    }
}

ssize_t AsyncFileFlush::flush(FlushBuffer* flushBuffer) {
    ssize_t written = 0;
    if(log_fd != -1 && flushBuffer->length() > 0) {
        written = write(log_fd, flushBuffer->ptr(), flushBuffer->length());
    }
    delete flushBuffer;
    return written;
}

bool AsyncFileFlush::async_flush(FlushBuffer* flushBuffer) {
    std::unique_lock<std::mutex> lck_async_flush(async_mtx);
    if (exit) {
        delete flushBuffer;
        return false;
    }
    async_buffer.push_back(flushBuffer);
    async_condition.notify_all();
    return true;
}

void AsyncFileFlush::stopFlush() {
    exit = true;
    async_condition.notify_all();
    async_thread.join();
}
