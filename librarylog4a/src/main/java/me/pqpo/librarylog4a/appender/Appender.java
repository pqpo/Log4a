package me.pqpo.librarylog4a.appender;

/**
 * Created by pqpo on 2017/11/16.
 */
public interface Appender {

    void append(int logLevel, String tag, String msg);
    void flush();
    void release();

}
