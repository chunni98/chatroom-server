#ifndef _SHACHI_LOGGER_H
#define _SHACHI_LOGGER_H
/******************************************************************************
 *     Copyright:  (C) 2023 shachi
 *                 All rights reserved.
 *
 *      Filename:  klogger.h
 *   Description:  包装 printf 实现的日志宏。定义宏 NDEBUG 关闭。
 *
 *       Version:  0.1.0
 *        Author:  shachi <shachi1758@outlook.com>
 *     ChangeLog:  1. 2023-02-24 09:53:10 创建文件
 *
 *****************************************************************************/

// 日志模块开关
#ifndef NDEBUG

#include <time.h>
#include <stdio.h>

#define COLOR_RED "\033[1;31m"
#define COLOR_BLUE "\033[1;34m"
#define COLOR_NONE "\033[0m"

#define KINFO(fmt, args...)\
do{\
    time_t t;\
    struct tm *ti;\
    time(&t);\
    ti = localtime(&t);\
    printf("%d-%d-%d %d:%d:%d ",ti->tm_year + 1900,ti->tm_mon+1,ti->tm_mday,ti->tm_hour,ti->tm_min,ti->tm_sec);\
    printf("[%s:%d->%s] ", __FILE__,__LINE__,__func__);\
    printf(COLOR_BLUE "INFO:" COLOR_NONE);\
    printf(fmt, ##args);\
}while(0)

#define KERROR(fmt, args...)\
do{\
    time_t t;\
    struct tm *ti;\
    time(&t);\
    ti = localtime(&t);\
    printf("%d-%d-%d %d:%d:%d ",ti->tm_year + 1900,ti->tm_mon+1,ti->tm_mday,ti->tm_hour,ti->tm_min,ti->tm_sec);\
    printf("[%s:%d->%s] ", __FILE__,__LINE__,__func__);\
    printf(COLOR_RED "ERROR:" COLOR_NONE);\
    printf(fmt, ##args);\
}while(0)

// 裸机简易 LOG
#define kdebug(fmt,args...) printf (COLOR_BLUE fmt COLOR_NONE,##args)
#define kdebugX(level,fmt,args...) if (DEBUG>=level) printf(fmt,##args);

#else

#define KINFO(fmt,args...)
#define KERROR(fmt, args...)
#define kdebug(fmt,args...)
#define kdebugX(level,fmt,args...)

#endif // NDEBUG

#endif // _SHACHI_LOGGER_H

