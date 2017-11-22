package me.pqpo.log4a;

import android.app.Application;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import me.pqpo.librarylog4a.Level;
import me.pqpo.librarylog4a.Log4a;
import me.pqpo.librarylog4a.LogData;
import me.pqpo.librarylog4a.Logger;
import me.pqpo.librarylog4a.appender.AndroidAppender;
import me.pqpo.librarylog4a.appender.FileAppender;
import me.pqpo.librarylog4a.interceptor.Interceptor;

/**
 * Created by pqpo on 2017/11/21.
 */

public class App extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
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


        File log = FileUtils.getLogDir(this);
        String buffer_path = log.getAbsolutePath() + File.separator + ".logCache";
        String log_path = log.getAbsolutePath() + File.separator + "log.txt";
        FileAppender.Builder fileBuild = new FileAppender.Builder()
                .setLevel(level)
                .setBufferFilePath(buffer_path)
                .setLogFilePath(log_path)
                .setBufferSize(4063)
                .addInterceptor(wrapInterceptor)
//                .setFormatter(new FileAppender.Formatter() {
//                    @Override
//                    public String format(int logLevel, String tag, String msg) {
//                        SimpleDateFormat sdf = new SimpleDateFormat("MM-dd HH:mm:ss", Locale.getDefault());
//                        return String.format("%s %s/%s: %s\n", sdf.format(new Date()), Level.getShortLevelName(logLevel), tag, msg);
//                    }
//                })
                ;
        Logger logger = new Logger.Builder()
//                .enableAndroidAppender(androidBuild)
                .enableFileAppender(fileBuild)
                .create();
        Log4a.setLogger(logger);

    }
}
