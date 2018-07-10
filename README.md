# Log4a

(English | [中文](README_CN.md))

    Log4a is an mmap based, high-performance, highly available Android log collection framework

## WHY Log4a:

**If you like, welcome to star/fork it or follow me.**

Log4a uses file mapping memory(mmap) as a cache, that the integrity of the log can be guaranteed without sacrificing performance.
The log will be first written to file mapping memory, based on the mmap feature, even if the user kill the process, 
the log data will not be lost, and will write back to the log file at the next time Log4a is initialized.

For mobile developers, with some users's online feedback which is difficult to reproduce the problem, analysis of the log is sometimes necessary to solve the problem.
However, the collection of logs has always been a pain point, that is, performance and log integrity can not have both.
To achieve high-performance log collection, it is bound to use a lot of memory, the first write the log into memory, 
and then at the right time to write the log which in memory to the file system (flush)
If flushing before the user kill the process, then the contents of the memory will be lost.
Real-time write log to file can be written to ensure the integrity of the log, but the write file is a IO operation, 
involves the user mode and kernel state switch, compared to write directly to the memory will be more time-consuming, 
frequent writes in the UI thread file will cause Caton , Affect the user experience.

## HOW:

Use the same way as the android.util.Log, the difference is that you need a simple configuration, 
of course, also reserves a wealth of interfaces for expansion of use, 
more advanced configuration can view [Sample](https://github.com/pqpo/Log4a/blob/7d92dc4ad244c8af80d0c5ce6e02d7bff53277b8/app/src/main/java/me/pqpo/log4a/LogInit.java#L23);

1. Add dependencies in the build.gradle file：
```groovy
allprojects {
	repositories {
		maven { url 'https://jitpack.io' }
	}
}
	
dependencies {
    compile 'com.github.pqpo:Log4a:{last-version}'
}
```

2. Set and initialize Log4a::
```java
AndroidAppender.Builder androidBuild = new AndroidAppender.Builder();

File log = FileUtils.getLogDir(context);
String log_path = log.getAbsolutePath() + File.separator + "log.txt";
FileAppender.Builder fileBuild = new FileAppender.Builder(context)
                          .setLogFilePath(log_path);
                          
Logger logger = new Logger.Builder()
          .enableAndroidAppender(androidBuild)
          .enableFileAppender(fileBuild)
          .create();
          
Log4a.setLogger(logger);
```

3. Use the same way as the android.util.Log：
```java
Log4a.i(TAG, "Hello，Log4a!");
```

4. Choose to refresh the cache ** or ** free the memory at the right time 
```java
Log4a.flush();
//or
Log4a.release();
```

## Performance Testing

you can download [Sample APK](art/log4a_sample_v1.0.0.apk) for testing on your device.。

The following are the test cases for writing 1w logs in Google Pixel and Moto X:

|Google Pixel|Moto X|
|:---:|:---:|
|![](art/pixel1.jpg)|![](art/motox1.jpg)|

Google pixel on the chart test data as follows (sorted by elapsed time):

|device|test type|time consumed|complete?|persistent?|complete after kill the process?|
|:---:|:---:|:---:|:---:|:---:|:---:|
|Google Pixel|Mem|13ms|Y|N|N|
|Google Pixel|Log4a|50ms|Y|Y|Y|
|Google Pixel|File with Buffer|61ms|Y|Y|N|
|Google Pixel|Android Log|184ms|N|N|N|
|Google Pixel|File no Buffer|272ms|Y|Y|Y|

Log4a can be seen writing the performance of the log behind direct write memory, 
and the use of BufferOutputStream write the same file, in fact, in order to ensure multi-threaded security, 
Log4a write mmap memory are locked, unlocked Case can be closer to write the speed of memory (interested are free to test).
BufferOutputStream is the first data cache in memory, and then refresh the file, 
if the power was removed before the refresh or kill the process, 
then the memory data will be lost can not be restored. Log4a will restore the log file the next time you start to ensure the integrity of the log.

## CHANGE LOG

see [change log file](CHAGELOG.md)

## Thanks

- [Tencent/mars](https://github.com/Tencent/mars)
- [XLog](https://github.com/elvishew/xLog)

---

## About Me：

- Email：    pqponet@gmail.com
- GitHub：  [pqpo](https://github.com/pqpo)
- Blog：    [pqpo's notes](https://pqpo.me)
- Twitter: [Pqponet](https://twitter.com/Pqponet)
- WeChat: pqpo_me

<img src="art/qrcode_for_gh.jpg" width="200">

## License

    Copyright 2017 pqpo
    
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
    
       http://www.apache.org/licenses/LICENSE-2.0
    
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

