//
//  ViewController.m
//  ALog
//
//  Created by JackSun on 2018/3/9.
//  Copyright © 2018年 JackSun. All rights reserved.
//

#import "ViewController.h"
#import "ALogBuffer.h"
#import "KKLog.h"


@interface ViewController ()

@property (nonatomic,strong) ALogBuffer *logBuffer;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
   
    
    [self KKlog];
    [self runMMap];

    
//    NSTimer *timer= [NSTimer scheduledTimerWithTimeInterval:0.001 target:self selector:@selector(myLog) userInfo:nil repeats:YES];
    
 }

- (void)runMMap
{
    NSLog(@"hellommap%@",@1111);
    self.logBuffer = [[ALogBuffer alloc] init];
    
    [self.logBuffer initLogEvn];
    for (int i=0; i<100000; i++) {
        [self myLog];
    }
    [self.logBuffer flushAsync];
    [self.logBuffer releaseNative];
    NSLog(@"hellommap%@",@1111);
}



static int i =0;
- (void)myLog
{
//    NSDate *currentDate = [NSDate date];//获取当前时间，日期
//    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];// 创建一个时间格式化对象
//    [dateFormatter setDateFormat:@"YYYY/MM/dd hh:mm:ss SS "];//设定时间格式,这里可以设置成自己需要的格式
//    NSString *dateString = [dateFormatter stringFromDate:currentDate];
//
   
//    [self.logBuffer writeLogWithString:dateString];
    i++;
    [self.logBuffer writeLogWithString:[NSString stringWithFormat:@"%@ \n",@(i)] ];
  
}

- (void)KKlog
{
    NSLog(@"hello%@",@222);
    for (int i=0; i<100000; i++) {
        [KKLog logLevel:LOGLEVELW LogInfo:[NSString stringWithFormat:@"%@",@(i)] ];
    }
     NSLog(@"hello%@",@222);
}




- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


@end
