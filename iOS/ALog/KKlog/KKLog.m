//
//  KKLog.m
//  KKLog
//
//  Created by JackSun on 15/4/9.
//  Copyright (c) 2015年 Coneboy_K. All rights reserved.
//

#import "KKLog.h"
#import <UIKit/UIKit.h>
#include <sys/types.h>
#include <sys/sysctl.h>

#ifdef DEBUG
//Debug默认记录的日志等级为LOGLEVELD。
static KKLogLevel LogLevel = LOGLEVELD;
#else
//正常模式默认记录的日志等级为LOGLEVELI。
static KKLogLevel LogLevel = LOGLEVELI;
#endif

static NSString *logFilePath = nil;
static NSString *logDic      = nil;
static NSString *crashDic    = nil;

static NSFileHandle *file =nil;
// 定义删除几天前的日志
const int k_preDaysToDelLog = 3;

// 打印队列
static dispatch_once_t logQueueCreatOnce;
static dispatch_queue_t k_operationQueue;


@interface KKLog(privatedMethod)
+ (void)logvLevel:(KKLogLevel)level Format:(NSString*)format VaList:(va_list)args;
+ (NSString*)KKStringFromLogLevel:(KKLogLevel)logLevel;
+ (NSString*)KKLogFormatPrefix:(KKLogLevel)logLevel;
@end



@implementation KKLog

+ (NSDate *)nowBeijingTime
{
    NSTimeZone *AA = [NSTimeZone timeZoneWithAbbreviation:@"GMT+0800"];
    NSInteger seconds = [AA secondsFromGMTForDate: [NSDate date]];
    return [NSDate dateWithTimeInterval: seconds sinceDate: [NSDate date]];
}


+ (NSString*)KKStringFromLogLevel:(KKLogLevel)logLevel
{
    switch (logLevel)
    {
        case LOGLEVELV: return @"VEND";
        case LOGLEVELD: return @"DEBUG";
        case LOGLEVELI: return @"INFO";
        case LOGLEVELW: return @"WARNING";
        case LOGLEVELE: return @"ERROR";
    }
    return @"";
}

+ (NSString*)KKLogFormatPrefix:(KKLogLevel)logLevel
{
    return [NSString stringWithFormat:@"[%@] ", [KKLog KKStringFromLogLevel:logLevel]];
}


+ (void)logIntial
{
    if (!logFilePath)
    {
        NSString *documentsDirectory = [NSSearchPathForDirectoriesInDomains (NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
        NSString *logDirectory       = [documentsDirectory stringByAppendingString:@"/log/"];
        NSString *crashDirectory     = [documentsDirectory stringByAppendingString:@"/log/"];
        
        if (![[NSFileManager defaultManager] fileExistsAtPath:logDirectory]) {
            [[NSFileManager defaultManager] createDirectoryAtPath:logDirectory
                                      withIntermediateDirectories:YES
                                                       attributes:nil
                                                            error:nil];
        }
        
        if (![[NSFileManager defaultManager] fileExistsAtPath:crashDirectory]) {
            [[NSFileManager defaultManager] createDirectoryAtPath:crashDirectory
                                      withIntermediateDirectories:YES
                                                       attributes:nil
                                                            error:nil];
        }
        
        logDic   = logDirectory;
        crashDic = crashDirectory;
        
        NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
        [dateFormatter setDateFormat:@"yyyy-MM-dd"];
        dateFormatter.timeZone = [NSTimeZone timeZoneWithAbbreviation:@"GMT+0800"];;
        NSString *fileNamePrefix = [dateFormatter stringFromDate:[NSDate date]];
        NSString *fileName = [NSString stringWithFormat:@"KK_log_%@.logtraces.txt", fileNamePrefix];
        NSString *filePath = [logDirectory stringByAppendingPathComponent:fileName];
        
        logFilePath = filePath;
#if DEBUG
        NSLog(@"LogPath: %@", logFilePath);
#endif
        //create file if it doesn't exist
        if(![[NSFileManager defaultManager] fileExistsAtPath:filePath])
            [[NSFileManager defaultManager] createFileAtPath:filePath contents:nil attributes:nil];
        
        //删除过期的日志
        NSDate *prevDate = [[NSDate date] dateByAddingTimeInterval:-60*60*24*k_preDaysToDelLog];
        NSDateComponents *components = [[NSCalendar currentCalendar] components:NSYearCalendarUnit|NSMonthCalendarUnit|NSDayCalendarUnit fromDate:prevDate];
        [components setHour:0];
        [components setMinute:0];
        [components setSecond:0];
        
        //要删除三天以前的日志（0点开始）
        NSDate *delDate = [[NSCalendar currentCalendar] dateFromComponents:components];
        NSArray *logFiles = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:logDic error:nil];
        for (NSString *file in logFiles)
        {
            NSString *fileName = [file stringByReplacingOccurrencesOfString:@".logtraces.txt" withString:@""];
            fileName = [fileName stringByReplacingOccurrencesOfString:@"KK_log_" withString:@""];
            NSDate *fileDate = [dateFormatter dateFromString:fileName];
            if (nil == fileDate)
            {
                continue;
            }
            if (NSOrderedAscending == [fileDate compare:delDate])
            {
                [[NSFileManager defaultManager] removeItemAtPath:[logDic stringByAppendingString:file] error:nil];
            }
        }
    }
    
    dispatch_once(&logQueueCreatOnce, ^{
        k_operationQueue =  dispatch_queue_create("com.coneboy.app.operationqueue", DISPATCH_QUEUE_SERIAL);
    });
    
    
}

+ (void)setLogLevel:(KKLogLevel)level
{
    LogLevel = level;
}



+ (void)logvLevel:(KKLogLevel)level Format:(NSString *)format VaList:(va_list)args
{
    __block NSString *formatTmp = format;
    
    dispatch_async(k_operationQueue, ^{
        
        if (level >= LogLevel)
        {
//            formatTmp = [[KKLog KKLogFormatPrefix:level] stringByAppendingString:formatTmp];
            NSString *contentStr = [[NSString alloc] initWithFormat:formatTmp arguments:args];
            NSString *contentN = [contentStr stringByAppendingString:@"\n"];
            NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
            [dateFormatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
            NSString *content = [NSString stringWithFormat:@"%@", contentN];
            
            //append text to file (you'll probably want to add a newline every write)
            if (!file) {
                 file   = [NSFileHandle fileHandleForUpdatingAtPath:logFilePath];
                 [file seekToEndOfFile];
            }
           
            [file writeData:[content dataUsingEncoding:NSUTF8StringEncoding]];
             
#ifdef DEBUG
//            NSLog(@"%@", content);
#endif
            
            formatTmp = nil;
        }
        
    });
    
    
    
}

+ (void)logCrash:(NSException *)exception
{
    if (nil == exception)
    {
        return;
    }
#ifdef DEBUG
    NSLog(@"CRASH: %@", exception);
    NSLog(@"Stack Trace: %@", [exception callStackSymbols]);
#endif
    // Internal error reporting
    if (!crashDic) {
        NSString *documentsDirectory = [NSSearchPathForDirectoriesInDomains (NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];

        NSString *crashDirectory = [documentsDirectory stringByAppendingString:@"/log/"];
        crashDic = crashDirectory;
    }
    
    NSString *fileName = [NSString stringWithFormat:@"CRASH_%@.log", [[KKLog nowBeijingTime] description]];
    NSString *filePath = [crashDic stringByAppendingString:fileName];
    NSString *content = [[NSString stringWithFormat:@"CRASH: %@\n", exception] stringByAppendingString:[NSString stringWithFormat:@"Stack Trace: %@\n", [exception callStackSymbols]]];
    
    
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSArray *languages = [defaults objectForKey:@"AppleLanguages"];
    NSString *phoneLanguage = [languages objectAtIndex:0];
    
    content = [content stringByAppendingString:[NSString stringWithFormat:@"iPhone:%@  iOS Version:%@ Language:%@",[KKLog platformString], [[UIDevice currentDevice] systemVersion],phoneLanguage]];
    NSError *error = nil;
    [content writeToFile:filePath atomically:YES encoding:NSUTF8StringEncoding error:&error];
    
    if (error) {
#if DEBUG
        NSLog(@"error is %@",error);
#endif
        [KKLog logE:@"CRASH LOG CREAT ERR INFO IS %@",error];
    }
    
}

+ (void)logLevel:(KKLogLevel)level LogInfo:(NSString *)format, ...
{
    va_list args;
    va_start(args, format);
    [KKLog logvLevel:level Format:format VaList:args];
    va_end(args);
}

+ (void)logV:(NSString *)format, ...
{
    va_list args;
    va_start(args, format);
    [KKLog logvLevel:LOGLEVELV Format:format VaList:args];
    va_end(args);
}

+ (void)logD:(NSString *)format, ...
{
    va_list args;
    va_start(args, format);
    [KKLog logvLevel:LOGLEVELD Format:format VaList:args];
    va_end(args);
}

+ (void)logI:(NSString *)format, ...
{
    va_list args;
    va_start(args, format);
    [KKLog logvLevel:LOGLEVELI Format:format VaList:args];
    va_end(args);
}

+ (void)logW:(NSString *)format, ...
{
    va_list args;
    va_start(args, format);
    [KKLog logvLevel:LOGLEVELW Format:format VaList:args];
    va_end(args);
}

+ (void)logE:(NSString *)format, ...
{
    va_list args;
    va_start(args, format);
    [KKLog logvLevel:LOGLEVELE Format:format VaList:args];
    va_end(args);
}



+ (NSDate *)getLogTime:(NSString *)logName
{
    NSString *fileName = [logName stringByReplacingOccurrencesOfString:@".logtraces.txt" withString:@""];
    fileName = [fileName stringByReplacingOccurrencesOfString:@"KK_log_" withString:@""];
    
    fileName = [fileName stringByReplacingOccurrencesOfString:@".log" withString:@""];
    fileName = [fileName stringByReplacingOccurrencesOfString:@"KK_FileTransport_" withString:@""];
    
    fileName = [fileName stringByReplacingOccurrencesOfString:@"CRASH_" withString:@""];
    
    fileName = [fileName stringByReplacingOccurrencesOfString:@"KK_Conference_" withString:@""];
    NSRange rangeConf = [fileName rangeOfString:@"["];
    if (rangeConf.location != NSNotFound)
    {
        fileName = [fileName substringToIndex:rangeConf.location];
    }
    
    NSRange rangeCrash = [fileName rangeOfString:@" "];
    if (rangeCrash.location != NSNotFound)
    {
        fileName = [fileName substringToIndex:rangeCrash.location];
    }
    
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
    [dateFormatter setDateFormat:@"yyyy-MM-dd"];
    dateFormatter.timeZone = [NSTimeZone timeZoneWithAbbreviation:@"GMT+0800"];
    NSDate *fileDate = [dateFormatter dateFromString:fileName];
    return fileDate;
}

+ (NSString *)platform
{
    size_t size;
    sysctlbyname("hw.machine", NULL, &size, NULL, 0);
    char *machine = malloc(size);
    sysctlbyname("hw.machine", machine, &size, NULL, 0);
    NSString *platform = [NSString stringWithUTF8String:machine];
    free(machine);
    return platform;
}

+ (NSString *)platformString
{
    NSString *platform = [KKLog platform];
    if ([platform isEqualToString:@"iPhone1,1"])    return @"iPhone 1G";
    if ([platform isEqualToString:@"iPhone1,2"])    return @"iPhone 3G";
    if ([platform isEqualToString:@"iPhone2,1"])    return @"iPhone 3GS";
    if ([platform isEqualToString:@"iPhone3,1"])    return @"iPhone 4 (GSM)";
    if ([platform isEqualToString:@"iPhone3,2"])    return @"iPhone 4 (GSM Rev A)";
    if ([platform isEqualToString:@"iPhone3,3"])    return @"iPhone 4 (CDMA)";
    if ([platform isEqualToString:@"iPhone4,1"])    return @"iPhone 4S";
    if ([platform isEqualToString:@"iPhone5,1"])    return @"iPhone 5 (GSM)";
    if ([platform isEqualToString:@"iPhone5,2"])    return @"iPhone 5 (GSM+CDMA)";
    if ([platform isEqualToString:@"iPhone5,3"])    return @"iPhone 5c (GSM)";
    if ([platform isEqualToString:@"iPhone5,4"])    return @"iPhone 5c (GSM+CDMA)";
    if ([platform isEqualToString:@"iPhone6,1"])    return @"iPhone 5s (GSM)";
    if ([platform isEqualToString:@"iPhone6,2"])    return @"iPhone 5s (GSM+CDMA)";
    if ([platform isEqualToString:@"iPod1,1"])      return @"iPod Touch 1G";
    if ([platform isEqualToString:@"iPod2,1"])      return @"iPod Touch 2G";
    if ([platform isEqualToString:@"iPod3,1"])      return @"iPod Touch 3G";
    if ([platform isEqualToString:@"iPod4,1"])      return @"iPod Touch 4G";
    if ([platform isEqualToString:@"iPod5,1"])      return @"iPod Touch 5G";
    if ([platform isEqualToString:@"iPad1,1"])      return @"iPad";
    if ([platform isEqualToString:@"iPad2,1"])      return @"iPad 2 (WiFi)";
    if ([platform isEqualToString:@"iPad2,2"])      return @"iPad 2 (GSM)";
    if ([platform isEqualToString:@"iPad2,3"])      return @"iPad 2 (CDMA)";
    if ([platform isEqualToString:@"iPad2,5"])      return @"iPad Mini (WiFi)";
    if ([platform isEqualToString:@"iPad2,6"])      return @"iPad Mini (GSM)";
    if ([platform isEqualToString:@"iPad2,7"])      return @"iPad Mini (GSM+CDMA)";
    if ([platform isEqualToString:@"iPad3,1"])      return @"iPad 3 (WiFi)";
    if ([platform isEqualToString:@"iPad3,2"])      return @"iPad 3 (GSM+CDMA)";
    if ([platform isEqualToString:@"iPad3,3"])      return @"iPad 3 (GSM)";
    if ([platform isEqualToString:@"iPad3,4"])      return @"iPad 4 (WiFi)";
    if ([platform isEqualToString:@"iPad3,5"])      return @"iPad 4 (GSM)";
    if ([platform isEqualToString:@"iPad3,6"])      return @"iPad 4 (GSM+CDMA)";
    if ([platform isEqualToString:@"i386"])         return @"Simulator";
    if ([platform isEqualToString:@"x86_64"])       return @"Simulator";
    
    return platform;
}


@end
