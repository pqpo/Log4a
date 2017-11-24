package me.pqpo.log4a;

import android.app.Application;

/**
 * Created by pqpo on 2017/11/21.
 */
public class App extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        LogInit.init(this);
    }
}
