/******************************************************************************
 *     Copyright:  (C) 2023 shachi
 *                 All rights reserved.
 *
 *      Filename:  ./incl/usr.h
 *   Description:  This file
 *
 *       Version:  0.1.0
 *        Author:  shachi <shachi1758@outlook.com>
 *     ChangeLog:  1. 2023-03-29 12:09:23 创建文件
 *
 *****************************************************************************/
#ifndef _USR_H
#define _USR_H

#include <inttypes.h>
#include <stdbool.h>

/**
 * Brief 保存用户信息
 */
struct usr{
    int32_t  uid;   //< 用户编号
    int32_t  sfd;   //< 用户套接字
    char name[20];  //< 昵称
    char psw[20];   //< 密码
    bool state;     //< 登录状态
    //char data[512];     //< chat info
    const char *data;
    int32_t gid;
};

#endif //_USR_H
