/******************************************************************************
 *     Copyright:  (C) 2023 shachi
 *                 All rights reserved.
 *
 *      Filename:  ./incl/server.h
 *   Description:  This file
 *
 *       Version:  0.1.0
 *        Author:  shachi <shachi1758@outlook.com>
 *     ChangeLog:  1. 2023-03-29 11:47:23 创建文件
 *
 *****************************************************************************/

#ifndef _SERVER_H
#define _SERVER_H
#include <inttypes.h>
#define ERROR 0
#define LOGIN 1
#define LOGON 2
#define QUIT  3
#define ADDFRIEND 4
#define DELFRIEND 5
#define SECRET_CHAT 6
#define GROUP_CHAT 7
#define GET_FRIENDS 8
#define GET_GROUP 9
#define CREATE_GROUP 10
#define GROUP_ADD 11
#define GROUP_DEL 12
#define DEL_GROUP 13

int32_t run();


#endif
