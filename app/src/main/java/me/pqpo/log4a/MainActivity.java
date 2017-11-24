package me.pqpo.log4a;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

import java.util.List;
import java.util.Locale;

import me.pqpo.librarylog4a.Log4a;
import me.pqpo.librarylog4a.Logger;
import me.pqpo.librarylog4a.appender.Appender;
import me.pqpo.librarylog4a.appender.FileAppender;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";

    EditText etContent;
    EditText etThread;
    Button btnWrite;
    Button btnTest;
    TextView tvTest;
    EditText etTimes;

    RadioGroup radioGroup;
    boolean testing = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        etThread = findViewById(R.id.et_thread);
        etContent = findViewById(R.id.et_content);
        btnWrite = findViewById(R.id.btn_write);
        btnTest = findViewById(R.id.btn_test);
        tvTest = findViewById(R.id.tv_test);
        etTimes = findViewById(R.id.et_times);
        radioGroup = findViewById(R.id.rg_log);

        btnWrite.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                int threads = Integer.valueOf(etThread.getText().toString());
                if (threads > 200) {
                    Toast.makeText(getApplicationContext(), "Do not exceed 200 threads", Toast.LENGTH_SHORT).show();
                    return;
                }
                final String str = etContent.getText().toString();
                for (int i=0; i<threads; i++) {
                    new Thread(){
                        @Override
                        public void run() {
                            super.run();
                            Log4a.i(TAG, str);
                        }
                    }.start();
                }
                tvTest.setText("done!\nlog file path:" + getLogPath());
            }
        });

        btnTest.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (!testing) {
                    tvTest.setText("start testing");
                    testing = true;
                    int times = Integer.valueOf(etTimes.getText().toString());
                    if (radioGroup.getCheckedRadioButtonId() == R.id.rb_log4a) {
                        long currentTimeMillis = System.currentTimeMillis();
                        for (int i = 0; i < times; i++) {
                            Log4a.i(TAG, "log-" + i);
                        }
                        tvTest.setText(String.format(Locale.getDefault(),
                                "Log4a prints %d logs spend: %d ms\nlog file path: %s",
                                times,
                                System.currentTimeMillis() - currentTimeMillis,
                                getLogPath()));
                    } else if (radioGroup.getCheckedRadioButtonId() == R.id.rb_android_log) {
                        long currentTimeMillis = System.currentTimeMillis();
                        for (int i = 0; i < times; i++) {
                            Log.i(TAG, "log-" + i);
                        }
                        tvTest.setText(String.format(Locale.getDefault(),
                                "Log prints %d logs spend: %d ms\nlog file path: %s",
                                times,
                                System.currentTimeMillis() - currentTimeMillis,
                                getLogPath()));
                    }
                    testing = false;
                } else {
                    Toast.makeText(getApplicationContext(), "testing", Toast.LENGTH_SHORT).show();
                }
            }
        });

    }

    public String getLogPath() {
        String logPath = "";
        Logger logger = Log4a.getLogger();
        List<Appender> appenderList = logger.getAppenderList();
        for (Appender appender : appenderList) {
            if (appender instanceof FileAppender) {
                FileAppender fileAppender = (FileAppender) appender;
                logPath = fileAppender.getLogPath();
                break;
            }
        }
        return logPath;
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
        Log4a.flush();
    }
}
