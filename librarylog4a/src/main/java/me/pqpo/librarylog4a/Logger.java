package me.pqpo.librarylog4a;

import java.util.ArrayList;
import java.util.List;

import me.pqpo.librarylog4a.appender.AndroidAppender;
import me.pqpo.librarylog4a.appender.Appender;
import me.pqpo.librarylog4a.appender.FileAppender;

/**
 * Created by pqpo on 2017/11/16.
 */
public class Logger {

    private List<Appender> appenderList = new ArrayList<>();

    protected Logger() {
    }

    public List<Appender> getAppenderList() {
        return appenderList;
    }

    public void addAppender(Appender appender) {
        if (appender != null) {
            appenderList.add(appender);
        }
    }

    protected void println(int priority, String tag, String msg) {
        if (appenderList == null) {
            return;
        }
        for (Appender appender : appenderList) {
            appender.append(priority, tag, msg);
        }
    }

    public void flush() {
        for (Appender appender : appenderList) {
            appender.flush();
        }
    }

    public void release() {
        for (Appender appender : appenderList) {
            appender.release();
        }
        appenderList.clear();
    }


    public static class Builder {

        private Logger logger;

        public Builder() {
            logger = new Logger();
        }

        public Builder addAppender(Appender appender) {
            logger.addAppender(appender);
            return this;
        }

        public Builder enableAndroidAppender(AndroidAppender.Builder builder) {
            logger.addAppender(builder.create());
            return this;
        }

        public Builder enableFileAppender(FileAppender.Builder builder) {
            if (builder != null) {
                logger.addAppender(builder.create());
            }
            return this;
        }

        public Logger create() {
            return logger;
        }

    }

}
