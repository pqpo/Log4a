//
// Created by pqpo on 2017/11/23.
//

#ifndef LOG4A_FILEFLUSH_H
#define LOG4A_FILEFLUSH_H

#include <sys/types.h>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include "FlushBuffer.h"

class AsyncFileFlush {

public:
    AsyncFileFlush(int log_fd);
    ~AsyncFileFlush();
    bool async_flush(FlushBuffer* flushBuffer);
    void stopFlush();

private:
    void async_log_thread();
    ssize_t flush(FlushBuffer* flushBuffer);

    bool exit = false;
    int log_fd;
    std::vector<FlushBuffer*> async_buffer;
    std::thread async_thread;
    std::condition_variable async_condition;
    std::mutex async_mtx;
};


#endif //LOG4A_FILEFLUSH_H
