//
//  AyncTest.m
//  ALog
//
//  Created by JackSun on 2018/3/9.
//  Copyright © 2018年 JackSun. All rights reserved.
//

#import "LogBrige.h"
#include "src/includes/LogBuffer.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>

// 定义日志刷新类库
static AsyncFileFlush *fileFlush = nullptr;
// 打开mmap
static char* openMMap(int buffer_fd, size_t buffer_size);

static void writeDirtyLogToFile(int buffer_fd);

// 初始化本地环境
static long initNative(const char *buffer_path_tmp,const char *log_path_tmp,int capacity,bool compress) {
    
    const char *buffer_path = buffer_path_tmp;
    const char *log_path = log_path_tmp;
    
    const size_t buffer_size = static_cast<size_t>(capacity);
    
    //
    int buffer_fd = open(buffer_path, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    int log_fd = open(log_path, O_RDWR|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    // buffer 的第一个字节会用于存储日志路径名称长度，后面紧跟日志路径，之后才是日志信息
    if (fileFlush == nullptr) {
        fileFlush = new AsyncFileFlush(log_fd);
    }
    char *buffer_ptr = openMMap(buffer_fd, buffer_size);
    bool map_buffer = true;
    //如果打开 mmap 失败，则降级使用内存缓存
    if(buffer_ptr == nullptr) {
        buffer_ptr = new char[capacity];
        map_buffer = false;
    }
    LogBuffer* logBuffer = new LogBuffer(buffer_ptr, buffer_size);
    //将buffer内的数据清0， 并写入日志文件路径
    logBuffer->initData((char *) log_path, strlen(log_path), compress);
    logBuffer->map_buffer = map_buffer;
    return reinterpret_cast<long>(logBuffer);
}

// 打开mmap
static char* openMMap(int buffer_fd, size_t buffer_size) {
    
    char* map_ptr = nullptr;
    if (buffer_fd != -1) {
        // 写脏数据
        writeDirtyLogToFile(buffer_fd);
        // 根据 buffer size 调整 buffer 文件大小
        ftruncate(buffer_fd, static_cast<int>(buffer_size));
        lseek(buffer_fd, 0, SEEK_SET);
        map_ptr = (char *) mmap(0, buffer_size, PROT_WRITE | PROT_READ, MAP_SHARED, buffer_fd, 0);
        if (map_ptr == MAP_FAILED) {
            map_ptr = nullptr;
        }
    }
    
    return map_ptr;
}

// 写脏页到文件
static void writeDirtyLogToFile(int buffer_fd) {
    struct stat fileInfo;
    if(fstat(buffer_fd, &fileInfo) >= 0) {
        size_t buffered_size = static_cast<size_t>(fileInfo.st_size);
        if(buffered_size > 0) {
            char *buffer_ptr_tmp = (char *) mmap(0, buffered_size, PROT_WRITE | PROT_READ, MAP_SHARED, buffer_fd, 0);
            if (buffer_ptr_tmp != MAP_FAILED) {
                LogBuffer tmp(buffer_ptr_tmp, buffered_size);
                size_t data_size = tmp.length();
                if (data_size > 0) {
                    char* log_path = tmp.getLogPath();
                    if (log_path != nullptr) {
                        int log_fd = open(log_path, O_RDWR|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
                        if(log_fd != -1) {
                            AsyncFileFlush tmpFlush(log_fd);
                            tmp.async_flush(&tmpFlush);
                        }
                        delete[] log_path;
                    }
                }
            }
        }
    }
}

// 写日志
static void writeNative(const char *log_tmp,int log_len_tmp , long ptr) {
    const char *log =log_tmp;
    int  log_len = log_len_tmp;
    LogBuffer* logBuffer = reinterpret_cast<LogBuffer*>(ptr);
    // 缓存写不下时异步刷新
    if (log_len >= logBuffer->emptySize()) {
        logBuffer->async_flush(fileFlush);
    }
    logBuffer->append(log, (size_t)log_len);
}

// 释放
static void releaseNative(long ptr) {
    
    LogBuffer* logBuffer = reinterpret_cast<LogBuffer*>(ptr);
    logBuffer->async_flush(fileFlush);
    delete logBuffer;
    if (fileFlush != nullptr) {
        delete fileFlush;
    }
    fileFlush = nullptr;
}

// 释放
static void  flushAsyncNative(long ptr) {
    
    LogBuffer* logBuffer = reinterpret_cast<LogBuffer*>(ptr);
    logBuffer->async_flush(fileFlush);
}


@interface LogBrige()

@property(nonatomic,assign) BOOL inited;

@end

@implementation LogBrige

- (instancetype)init
{
    self = [super init];
    if (self) {
        self.inited = NO;
    }
    return self;
}


- (long)initNative:(NSString *)bufferPath capacity:(int)capacity logPath:(NSString*)logPath needCompress:(BOOL)compress
{
    BOOL needInit = NO;
    @synchronized(self) {
      if (!self.inited) {
          needInit = YES;
          self.inited = YES;
      }
    }
    
    long willRetrun =0;
    if (needInit) {
        // 初始化数据
        NSString *bufferPathTmp = [bufferPath copy];
        const char *buffer_path_tmp = [bufferPathTmp UTF8String];
        
        NSString *logPathTmp = [logPath copy];
        const char *log_path_tmp = [logPathTmp UTF8String];
        
        bool compressTmp = compress? true:false;
        
        int finalCapacity = capacity<0? 1024*1024:capacity;
        
        willRetrun =  initNative(buffer_path_tmp,log_path_tmp,finalCapacity,compressTmp);
     }
 
    return willRetrun;
}

- (void)writeNativeWithPtr:(long)ptr withLonStr:(NSString *)lonStr
{
    NSString *lonStrTmp = [lonStr copy];
    const char *log_path_tmp = [lonStrTmp UTF8String];
    int log_len = (int)lonStr.length;

    writeNative(log_path_tmp,log_len,ptr);
}

- (void)flushAsyncNative:(long)ptr
{
    flushAsyncNative(ptr);
}


- (void)releaseNative:(long)ptr
{
    releaseNative(ptr);
}



@end
