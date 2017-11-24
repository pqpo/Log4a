package me.pqpo.librarylog4a.formatter;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import me.pqpo.librarylog4a.Level;

/**
 * Created by pqpo on 2017/11/24.
 */
public class DateFileFormatter implements Formatter{
    private SimpleDateFormat simpleDateFormat = null;
    private Date date = new Date();

    public DateFileFormatter() {
        this("MM-dd HH:mm:ss");
    }

    public DateFileFormatter(String pattern) {
        simpleDateFormat = new SimpleDateFormat(pattern, Locale.getDefault());
    }

    @Override
    public synchronized String format(int logLevel, String tag, String msg) {
        date.setTime(System.currentTimeMillis());
        return String.format("%s %s/%s: %s\n", simpleDateFormat.format(date), Level.getShortLevelName(logLevel), tag, msg);
    }
}
