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


class LogBuffer {
public:
    LogBuffer(char* ptr, size_t capacity, int log_fd);
    ~LogBuffer();

    char* ptr();
    char* dataCopy();
    size_t capacity();
    size_t size();
    size_t append(const char* log);
    ssize_t flush();
    void clear();
    void release();
    size_t emptySize();
    ssize_t flush(char *data);
    void async_flush();
    ssize_t flush(char *data, size_t str_len);

public:
    bool map_buffer = true;

private:
    char* buffer_ptr = nullptr;
    size_t buffer_capacity = 0;
    int log_file_fd = -1;
    bool exit = false;
    std::mutex log_mtx;

    std::vector<char*> async_buffer;
    std::thread async_thread;
    std::mutex async_mtx;
    std::condition_variable async_condition;

    void async_log_thread();
};


#endif //LOG4A_LOGBUFFER_H
