/******************************************************************************
 *     Copyright:  (C) 2023 shachi
 *                 All rights reserved.
 *
 *      Filename:  ./incl/database.h
 *   Description:  定义数据库的操作
 *
 *       Version:  0.1.0
 *        Author:  shachi <shachi1758@outlook.com>
 *     ChangeLog:  1. 2023-03-29 16:28:35 创建文件
 *
 *****************************************************************************/
#ifndef _DATABASE_H
#define _DATABASE_H
#include <stdbool.h>
#include "usr.h"
#include "group.h"

#define DATABASE_PATH "./etc/server.db"

// 创建表
bool create_tb(char sql[]);

// 根据 id 查询用户表
bool user_query(sqlite3 *db, struct usr *user);

//bool user_insert(struct usr *user);

// TODO: 删除、修改用户表


// TODO: 删除好友、添加好友

/**
 *   Description: 查询 user 用户的好友列表
 *    Input Args: user, 保存用户信息的结构体
 *   Output Args: friends, 保存好友信息的结构体数组
 *  Return Value: 成功返回 true，失败返回 false
 */
size_t friends_query(struct usr *user, struct usr friends[]);

// 更新用户表
// TODO:
bool update_user_name();
bool update_user_psw();
bool update_user_cfd_and_state(struct usr *user);

// insert user
bool add_user(struct usr *user);

// query friend
bool uid1_uid2(sqlite3 *db, int32_t uid1, int32_t uid2);

bool insert_friend(sqlite3 *db, int32_t uid1, int32_t uid2);
bool delete_friend(sqlite3 *db, int32_t uid1, int32_t uid2);

// 群聊——————————————————————————————————————————————————————————
// 获取所有聊天群

// 查询群聊信息，群聊不存在返回 false

// 查询用户加入的所有群聊的信息
size_t groups_query(struct usr *user, struct group groups[]);

// 创建群聊
// 成功返回群号，失败返回 -1
int32_t insert_group(struct usr *user);


// 获取群里所有 fd > 0 的用户的 fd
// 返回在线群友数
size_t get_online_user(int32_t gid, int32_t fds[]);

bool insert_group_user(int32_t gid, int32_t uid);
bool del_group_user(int32_t uida,int32_t gid, int32_t uidb);

#endif // _DATABASE_H
