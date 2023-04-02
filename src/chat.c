/******************************************************************************
 *     Copyright:  (C) 2023 shachi
 *                 All rights reserved.
 *
 *      Filename:  ./src/chat.c
 *   Description:  This file
 *
 *       Version:  0.1.0
 *        Author:  shachi <shachi1758@outlook.com>
 *     ChangeLog:  1. 2023-03-31 12:42:15 创建文件
 *
 *****************************************************************************/
#include <stdbool.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include "usr.h"
#include "klogger.h"
#include "json2data.h"
#include "database.h"


// if false user not exists
bool secret_chat(struct usr *from_user, struct usr *to_user)
{
    // get fd
    sqlite3 *db;

    if (sqlite3_open(DATABASE_PATH,&db) != SQLITE_OK) {
        KERROR("无法打开数据库 %s\n", sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }
    if (!user_query(db,to_user) ) {
        return false;
    }
    //kdebug("fuid:%d,ffd:%d\n",from_user->uid,from_user->sfd);
    //kdebug("tuid:%d,tfd:%d\n",to_user->uid,to_user->sfd);
    //kdebug("func:data:%s\n",to_user->data);

    // package data
    const char *buf = set_chat_info(from_user,to_user);

    send(to_user->sfd,buf,strlen(buf) +1,0);
    kdebug("send buf:%s\n",buf);

    sqlite3_close(db);
    return true;
}

// 成功返回 true ，失败 返回 false
bool group_chat(struct usr *user)
{
    // 获取群聊里的所有 fd 不是 -1 的人的 fd
    //<= 0 代表没人在线，则不发送。
    int32_t fds[50];
    size_t len = get_online_user(user->gid, fds);
    if (len <= 0) {
        return false;
    }

    // package data
    const char *buf = set_group_chat_info(user);
    kdebug("send buf:%s\n",buf);

    // 遍历数组，依次发送
    for (size_t i = 0; i < len; ++ i){
        // 自己不用发送
        if (i != user->sfd)
            send(fds[i],buf,strlen(buf) +1,0);
    }

    return true;
}
