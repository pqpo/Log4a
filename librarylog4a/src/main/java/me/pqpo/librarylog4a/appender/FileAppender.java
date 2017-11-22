package me.pqpo.librarylog4a.appender;


import java.util.ArrayList;
import java.util.List;

import me.pqpo.librarylog4a.Level;
import me.pqpo.librarylog4a.LogBuffer;
import me.pqpo.librarylog4a.interceptor.Interceptor;

/**
 * Created by pqpo on 2017/11/16.
 */
public class FileAppender extends AbsAppender {

    private LogBuffer logBuffer;

    private Formatter defaultFormatter = new Formatter() {
        @Override
        public String format(int logLevel, String tag, String msg) {
            return String.format("%s/%s: %s\n",  Level.getShortLevelName(logLevel), tag, msg);
        }
    };

    protected FileAppender(Builder builder) {
        logBuffer = new LogBuffer(builder.bufferFilePath, builder.bufferSize, builder.logFilePath);
        setMaxSingleLength(builder.bufferSize - 1);
        setLevel(builder.level);
        addInterceptor(builder.interceptors);
        setFormatter(builder.formatter);
    }

    public String getBufferPath() {
        return logBuffer.getBufferPath();
    }

    public int getBufferSize() {
        return logBuffer.getBufferSize();
    }

    public String getLogPath() {
        return logBuffer.getLogPath();
    }

    public void setFormatter(Formatter formatter) {
        if (formatter != null) {
            this.defaultFormatter = formatter;
        }
    }

    @Override
    void doAppend(int logLevel, String tag, String msg) {
        logBuffer.write(defaultFormatter.format(logLevel, tag, msg));
    }

    @Override
    public void flush() {
        super.flush();
        logBuffer.flushAsync();
    }

    @Override
    public void release() {
        super.release();
        logBuffer.release();
    }

    public static class Builder {

        private String bufferFilePath;
        private String logFilePath;
        private int bufferSize;
        private int level = Level.VERBOSE;
        private List<Interceptor> interceptors;
        private Formatter formatter;

        public Builder setBufferSize(int bufferSize) {
            this.bufferSize = bufferSize;
            return this;
        }

        public Builder setBufferFilePath(String bufferFilePath) {
            this.bufferFilePath = bufferFilePath;
            return this;
        }

        public Builder setLogFilePath(String logFilePath) {
            this.logFilePath = logFilePath;
            return this;
        }

        public Builder setLevel(int level) {
            this.level = level;
            return this;
        }

        public Builder addInterceptor(Interceptor interceptor) {
            if (interceptors == null) {
                interceptors = new ArrayList<>();
            }
            interceptors.add(interceptor);
            return this;
        }

        public Builder setFormatter(Formatter formatter) {
            this.formatter = formatter;
            return this;
        }

        public FileAppender create() {
            return new FileAppender(this);
        }

    }

    public interface Formatter {
        String format(int logLevel, String tag, String msg);
    }

}
