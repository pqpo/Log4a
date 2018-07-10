//
//  KKLog.h
//  KKLog
//
//  Created by JackSun on 15/4/9.
//  Copyright (c) 2015年 Coneboy_K. All rights reserved.
//

#import <Foundation/Foundation.h>

//Debug
#define KKLogD(desStr) [KKLog logD:[NSString stringWithFormat:@"Function:%s Line:%d Des:%@",__func__,__LINE__,desStr],@""];
//Info
#define KKLogI(desStr) [KKLog logI:[NSString stringWithFormat:@"Function:%s Line:%d Des:%@",__func__,__LINE__,desStr],@""];
//Warning
#define KKLogW(desStr) [KKLog logW:[NSString stringWithFormat:@"Function:%s Line:%d Des:%@",__func__,__LINE__,desStr],@""];
//Error
#define KKLogE(desStr) [KKLog logE:[NSString stringWithFormat:@"Function:%s Line:%d Des:%@",__func__,__LINE__,desStr],@""];



//日志等级
typedef enum
{
    LOGLEVELV = 0,  //wend
    LOGLEVELD = 1,  //Debug
    LOGLEVELI = 2,  //Info
    LOGLEVELW = 3,  //Warning
    LOGLEVELE = 4,  //Error
} KKLogLevel;

@interface KKLog : NSObject

/**
 *  log初始化函数，在系统启动时调用
 */
+ (void)logIntial;

/**
 *  设置要记录的log级别
 *
 *  @param level level 要设置的log级别
 */
+ (void)setLogLevel:(KKLogLevel)level;

/**
 *  记录系统crash的Log函数
 *
 *  @param exception 系统异常
 */
+ (void)logCrash:(NSException*)exception;

/**
 *  log记录函数
 *
 *  @param level  log所属的等级
 *  @param format 具体记录log的格式以及内容
 */
+ (void)logLevel:(KKLogLevel)level LogInfo:(NSString*)format,... NS_FORMAT_FUNCTION(2,3);

/**
 *  LOGLEVELV级Log记录函数
 *
 *  @param format format 具体记录log的格式以及内容
 */
+ (void)logV:(NSString*)format,... NS_FORMAT_FUNCTION(1,2);

/**
 *  LOGLEVELD级Log记录函数
 *
 *  @param format 具体记录log的格式以及内容
 */
+ (void)logD:(NSString*)format,... NS_FORMAT_FUNCTION(1,2);

/**
 *  LOGLEVELI级Log记录函数
 *
 *  @param format 具体记录log的格式以及内容
 */
+ (void)logI:(NSString*)format,... NS_FORMAT_FUNCTION(1,2);

/**
 *  LOGLEVELW级Log记录函数
 *
 *  @param format 具体记录log的格式以及内容
 */
+ (void)logW:(NSString*)format,... NS_FORMAT_FUNCTION(1,2);

/**
 *  LOGLEVELE级Log记录函数
 *
 *  @param format 具体记录log的格式以及内容
 */
+ (void)logE:(NSString*)format,... NS_FORMAT_FUNCTION(1,2);

@end
