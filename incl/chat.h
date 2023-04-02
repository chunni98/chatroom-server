/******************************************************************************
 *     Copyright:  (C) 2023 shachi
 *                 All rights reserved.
 *
 *      Filename:  ./incl/chat.h
 *   Description:  This file
 *
 *       Version:  0.1.0
 *        Author:  shachi <shachi1758@outlook.com>
 *     ChangeLog:  1. 2023-03-31 12:40:36 创建文件
 *
 *****************************************************************************/
#ifndef _CHAT_H
#define _CHAT_H
#include <stdbool.h>
#include <inttypes.h>
#include "usr.h"

// 私聊
extern bool secret_chat(struct usr *from_user, struct usr *to_user);

extern bool group_chat(struct usr *user);



#endif // _CHAT_H
