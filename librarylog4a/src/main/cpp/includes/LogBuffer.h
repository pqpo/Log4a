//
// Created by pqpo on 2017/11/16.
//

#ifndef LOG4A_LOGBUFFER_H
#define LOG4A_LOGBUFFER_H

#include <string>
#include <math.h>
#include <unistd.h>
#include <sys/mman.h>
#include <thread>
#include<vector>
#include <mutex>
#include <condition_variable>
#include "AsyncFileFlush.h"


class LogBuffer {
public:
    LogBuffer(char* ptr, size_t capacity);
    ~LogBuffer();

    char* ptr();
    char* dataCopy();
    size_t capacity();
    size_t size();
    size_t append(const char* log);
    void clear();
    void release();
    size_t emptySize();
    void async_flush(AsyncFileFlush *fileFlush);

public:
    bool map_buffer = true;

private:
    size_t offset = 0;
    char* buffer_ptr = nullptr;
    size_t buffer_capacity = 0;
    std::recursive_mutex log_mtx;

};


#endif //LOG4A_LOGBUFFER_H
