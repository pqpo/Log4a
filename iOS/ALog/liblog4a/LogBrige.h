//
//  AyncTest.h
//  ALog
//
//  Created by JackSun on 2018/3/9.
//  Copyright © 2018年 JackSun. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface LogBrige : NSObject

- (instancetype)init;


- (long)initNative:(NSString *)bufferPath capacity:(int)capacity logPath:(NSString*)logPath needCompress:(BOOL)compress;

- (void)writeNativeWithPtr:(long)ptr withLonStr:(NSString *)lonStr;

- (void)flushAsyncNative:(long)ptr;

- (void)releaseNative:(long)ptr;


@end
