//
//  ALogBuffer.m
//  ALog
//
//  Created by JackSun on 2018/3/9.
//  Copyright © 2018年 JackSun. All rights reserved.
//

#import "ALogBuffer.h"
#import "LogBrige.h"
@interface ALogBuffer ()


@property (nonatomic ,assign) long ptr;
@property (nonatomic ,assign) NSInteger bufferSize;

@property (nonatomic ,strong) NSString *logPath;
@property (nonatomic ,strong) NSString *bufferPath;

@property (nonatomic ,strong) LogBrige *logBrige;

@end

static int capacity_max = 4096;

@implementation ALogBuffer


/// 初始化系统数据
- (void)initLogEvn
{
    NSString *logDirectory       = [self createAndGetFilePath:@"/log/real/" fileName:@"1.txt"];
    NSString *logBufferDirectory  = [self createAndGetFilePath:@"/log/buffer/" fileName:@".logCache_1"];
    
    NSLog(@"LOG PATH =%@",logDirectory);
 
    
    if (!self.logBrige) {
     self.logBrige = [[LogBrige alloc] init];
       
      self.ptr =   [self.logBrige initNative:logBufferDirectory capacity:capacity_max logPath:logDirectory needCompress:NO];
    }
}

// 写日志
- (void)writeLogWithString:(NSString *)logStr
{
    [self.logBrige writeNativeWithPtr:self.ptr withLonStr:logStr];
}


// 刷新日志
- (void)flushAsync
{
    [self.logBrige flushAsyncNative:self.ptr];
}

- (void)releaseNative
{
    [self.logBrige releaseNative:self.ptr];
}



- (NSString *)createAndGetFilePath:(NSString *)path fileName:(NSString *)fileName
{
    NSString *documentsDirectory = [NSSearchPathForDirectoriesInDomains (NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString *dirTmp       = [documentsDirectory stringByAppendingString:path];
    NSString *filePathTmp       = [dirTmp stringByAppendingString:fileName];

    
    NSFileManager *fileManager = [NSFileManager defaultManager];

    BOOL isDir = NO;
   BOOL folderExits =  [fileManager fileExistsAtPath:dirTmp isDirectory:&isDir];
    
    if (!folderExits) {
        [fileManager createDirectoryAtPath:dirTmp withIntermediateDirectories:YES attributes:nil error:nil];
    }
     // todo 如果是目录就需要删除
    BOOL fileExits =  [fileManager fileExistsAtPath:filePathTmp];
    
    if(!fileExits){
        [fileManager createFileAtPath:filePathTmp contents:nil attributes:nil];
    }
    
    
    return filePathTmp;
}

 
@end
