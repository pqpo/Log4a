package me.pqpo.log4a;

import android.content.Context;

import java.io.File;

import me.pqpo.librarylog4a.Level;
import me.pqpo.librarylog4a.Log4a;
import me.pqpo.librarylog4a.LogData;
import me.pqpo.librarylog4a.Logger;
import me.pqpo.librarylog4a.appender.AndroidAppender;
import me.pqpo.librarylog4a.appender.FileAppender;
import me.pqpo.librarylog4a.formatter.DateFileFormatter;
import me.pqpo.librarylog4a.interceptor.Interceptor;

/**
 * Created by pqpo on 2017/11/24.
 */
public class LogInit {

    public static final int BUFFER_SIZE = 1024 * 400; //400k

    public static void init(Context context) {
        int level = Level.DEBUG;
        Interceptor wrapInterceptor = new Interceptor() {
            @Override
            public boolean intercept(LogData logData) {
                logData.tag = "Log4a-" + logData.tag;
                return true;
            }
        };
        AndroidAppender.Builder androidBuild = new AndroidAppender.Builder()
                .setLevel(level)
                .addInterceptor(wrapInterceptor);

        File log = FileUtils.getLogDir(context);
        String buffer_path = log.getAbsolutePath() + File.separator + ".logCache";
        String log_path = log.getAbsolutePath() + File.separator + "log.txt";
        FileAppender.Builder fileBuild = new FileAppender.Builder(context)
                .setLogFilePath(log_path)
                .setLevel(level)
                .addInterceptor(wrapInterceptor)
                .setBufferFilePath(buffer_path)
                .setFormatter(new DateFileFormatter())
                .setBufferSize(BUFFER_SIZE);
        Logger logger = new Logger.Builder()
                .enableAndroidAppender(androidBuild)
                .enableFileAppender(fileBuild)
                .create();
        Log4a.setLogger(logger);
    }

}
