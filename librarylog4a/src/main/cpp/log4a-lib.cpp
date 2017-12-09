#include <jni.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>

#include "includes/LogBuffer.h"

static const char* const kClassDocScanner = "me/pqpo/librarylog4a/LogBuffer";

static char* openMMap(int buffer_fd, size_t buffer_size);

static void writeDirtyLogToFile(int buffer_fd);

static AsyncFileFlush *fileFlush = nullptr;

static jlong initNative(JNIEnv *env, jclass type, jstring buffer_path_,
           jint capacity, jstring log_path_) {
    const char *buffer_path = env->GetStringUTFChars(buffer_path_, 0);
    const char *log_path = env->GetStringUTFChars(log_path_, 0);
    const size_t buffer_size = static_cast<size_t>(capacity);
    int buffer_fd = open(buffer_path, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    int log_fd = open(log_path, O_RDWR|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    // buffer 的第一个字节会用于存储日志路径名称长度，后面紧跟日志路径，之后才是日志信息
    if (strlen(log_path) > CHAR_MAX / 2) {
        jclass je = env->FindClass("java/lang/IllegalArgumentException");
        std::ostringstream oss;
        oss << "The length of log path must be less than " << CHAR_MAX / 2;
        env -> ThrowNew(je, oss.str().c_str());
        return 0;
    }
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
    env->ReleaseStringUTFChars(buffer_path_, buffer_path);
    env->ReleaseStringUTFChars(log_path_, log_path);
    LogBuffer* logBuffer = new LogBuffer(buffer_ptr, buffer_size);
    //将buffer内的数据清0， 并写入日志文件路径
    logBuffer->initData(log_path);
    logBuffer->map_buffer = map_buffer;
    return reinterpret_cast<long>(logBuffer);
}

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

static void writeDirtyLogToFile(int buffer_fd) {
    struct stat fileInfo;
    if(fstat(buffer_fd, &fileInfo) >= 0) {
        size_t buffered_size = static_cast<size_t>(fileInfo.st_size);
        if(buffered_size > 0) {
            char *buffer_ptr_tmp = (char *) mmap(0, buffered_size, PROT_WRITE | PROT_READ, MAP_SHARED, buffer_fd, 0);
            if (buffer_ptr_tmp != MAP_FAILED) {
                LogBuffer tmp(buffer_ptr_tmp, buffered_size);
                size_t data_size = tmp.dataSize();
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

static void writeNative(JNIEnv *env, jobject instance, jlong ptr,
            jstring log_) {
    const char *log = env->GetStringUTFChars(log_, 0);
    LogBuffer* logBuffer = reinterpret_cast<LogBuffer*>(ptr);
    size_t log_size = strlen(log);
    // 缓存写不下时异步刷新
    if (log_size >= logBuffer->emptySize()) {
        logBuffer->async_flush(fileFlush);
    }
    logBuffer->append(log);
    env->ReleaseStringUTFChars(log_, log);
}

static void releaseNative(JNIEnv *env, jobject instance, jlong ptr) {
    LogBuffer* logBuffer = reinterpret_cast<LogBuffer*>(ptr);
    logBuffer->async_flush(fileFlush);
    delete logBuffer;
    if (fileFlush != nullptr) {
        delete fileFlush;
    }
    fileFlush = nullptr;
}

static void  flushAsyncNative(JNIEnv *env, jobject instance, jlong ptr) {
    LogBuffer* logBuffer = reinterpret_cast<LogBuffer*>(ptr);
    logBuffer->async_flush(fileFlush);
}

static JNINativeMethod gMethods[] = {

        {
                "initNative",
                "(Ljava/lang/String;ILjava/lang/String;)J",
                (void*)initNative
        },

        {
                "writeNative",
                "(JLjava/lang/String;)V",
                 (void*)writeNative
        },

        {
                "flushAsyncNative",
                "(J)V",
                (void*)flushAsyncNative
        },

        {
                "releaseNative",
                "(J)V",
                (void*)releaseNative
        }

};

extern "C"
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return JNI_FALSE;
    }
    jclass classDocScanner = env->FindClass(kClassDocScanner);
    if(env -> RegisterNatives(classDocScanner, gMethods, sizeof(gMethods)/ sizeof(gMethods[0])) < 0) {
        return JNI_FALSE;
    }
    return JNI_VERSION_1_4;
}