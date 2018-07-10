//
//  ALogBuffer.h
//  ALog 入口文件
//
//  Created by JackSun on 2018/3/9.
//  Copyright © 2018年 JackSun. All rights reserved.
//

/**
 * 入口文件
    是以单例为标准
 */
#import <Foundation/Foundation.h>

@interface ALogBuffer : NSObject


- (void)initLogEvn;

// 写日志
- (void)writeLogWithString:(NSString *)logStr;

// 刷新日志
- (void)flushAsync;

- (void)releaseNative;

@end
