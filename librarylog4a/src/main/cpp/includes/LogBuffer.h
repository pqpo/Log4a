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

    void initData(const char *log_path);
    char* dataCopy();
    size_t dataSize();
    size_t append(const char* log);
    void clear();
    void release();
    size_t emptySize();
    char *getLogPath();
    bool async_flush(AsyncFileFlush *fileFlush);

public:
    bool map_buffer = true;

private:
    char* const buffer_ptr = nullptr;
    char* data_ptr = nullptr;
    char* write_ptr = nullptr;

    size_t buffer_size = 0;
    std::recursive_mutex log_mtx;

};


#endif //LOG4A_LOGBUFFER_H
