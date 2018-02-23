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
#include "FlushBuffer.h"
#include "LogBufferHeader.h"

using namespace log_header;

class LogBuffer {
public:
    LogBuffer(char* ptr, size_t capacity);
    ~LogBuffer();

    void initData(char *log_path, size_t log_path_len);
    size_t length();
    void setLength(size_t len);
    size_t append(const char* log, size_t len);
    void clear();
    void release();
    size_t emptySize();
    char *getLogPath();
    void async_flush(AsyncFileFlush *fileFlush);
    void flushToBuffer(FlushBuffer* flushBuffer);

public:
    bool map_buffer = true;

private:
    char* const buffer_ptr = nullptr;
    char* data_ptr = nullptr;
    char* write_ptr = nullptr;

    size_t buffer_size = 0;
    std::recursive_mutex log_mtx;

    LogBufferHeader logHeader;

};


#endif //LOG4A_LOGBUFFER_H
