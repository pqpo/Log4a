#include <jni.h>

#include <sys/stat.h>
#include <fcntl.h>

#include "includes/LogBuffer.h"

char *buildMmap(int buffer_fd, const size_t buffer_size, int log_fd);

extern "C"
JNIEXPORT jlong JNICALL
Java_me_pqpo_librarylog4a_LogBuffer_initNative(JNIEnv *env, jclass type, jstring buffer_path_,
                                              jint capacity, jstring log_path_) {
    const char *buffer_path = env->GetStringUTFChars(buffer_path_, 0);
    const char *log_path = env->GetStringUTFChars(log_path_, 0);
    const size_t buffer_size = static_cast<size_t>(capacity);
    int buffer_fd = open(buffer_path, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    int log_fd = open(log_path, O_RDWR|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);

    bool map_buffer = true;
    char *buffer_ptr = buildMmap(buffer_fd, buffer_size, log_fd);
    if(buffer_ptr == nullptr || buffer_ptr == MAP_FAILED) {
        buffer_ptr = new char[capacity];
        memset(buffer_ptr, '\0',buffer_size);
        map_buffer = false;
    }
    env->ReleaseStringUTFChars(buffer_path_, buffer_path);
    env->ReleaseStringUTFChars(log_path_, log_path);
    LogBuffer* logBuffer = new LogBuffer(buffer_ptr, buffer_size, log_fd);
    logBuffer->map_buffer = map_buffer;
    return reinterpret_cast<long>(logBuffer);
}

char *buildMmap(int buffer_fd, size_t buffer_size, int log_fd) {
    if(buffer_fd == -1) {
        return nullptr;
    }
    struct stat fileInfo;
    if(fstat(buffer_fd, &fileInfo) >= 0) {
        size_t buffered_size = static_cast<size_t>(fileInfo.st_size);
        if(buffered_size > 0) {
            char *buffer_ptr_tmp = (char *) mmap(0, buffered_size, PROT_WRITE | PROT_READ, MAP_SHARED, buffer_fd, 0);
            if (buffer_ptr_tmp != MAP_FAILED) {
                LogBuffer tmp(buffer_ptr_tmp, buffered_size, log_fd);
                tmp.async_flush();
            }
        }
    }
    ftruncate(buffer_fd, (int)(buffer_size));
    lseek(buffer_fd, 0, SEEK_SET);
    char* zero_data = new char[buffer_size];
    memset(zero_data, '\0', buffer_size);
    write(buffer_fd, zero_data, buffer_size);
    lseek(buffer_fd, 0, SEEK_SET);
    delete[] zero_data;
    return (char *) mmap(0, buffer_size, PROT_WRITE | PROT_READ, MAP_SHARED, buffer_fd, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_me_pqpo_librarylog4a_LogBuffer_writeNative(JNIEnv *env, jobject instance, jlong ptr,
                                                jstring log_) {
    const char *log = env->GetStringUTFChars(log_, 0);
    LogBuffer* logBuffer = reinterpret_cast<LogBuffer*>(ptr);
    size_t log_size = strlen(log);
    if (log_size >= logBuffer->emptySize()) {
        logBuffer->async_flush();
    }
    logBuffer->append(log);
    env->ReleaseStringUTFChars(log_, log);
}

extern "C"
JNIEXPORT void JNICALL
Java_me_pqpo_librarylog4a_LogBuffer_releaseNative(JNIEnv *env, jobject instance, jlong ptr) {
    LogBuffer* logBuffer = reinterpret_cast<LogBuffer*>(ptr);
    logBuffer->async_flush();
    delete logBuffer;
}

extern "C"
JNIEXPORT void JNICALL
Java_me_pqpo_librarylog4a_LogBuffer_flushAsyncNative(JNIEnv *env, jobject instance, jlong ptr) {
    LogBuffer* logBuffer = reinterpret_cast<LogBuffer*>(ptr);
    logBuffer->async_flush();
}

extern "C"
JNIEXPORT void JNICALL
Java_me_pqpo_librarylog4a_LogBuffer_flushImmediatelyNative(JNIEnv *env, jobject instance, jlong ptr) {
    LogBuffer* logBuffer = reinterpret_cast<LogBuffer*>(ptr);
    logBuffer->flush();
}