/******************************************************************************
 *     Copyright:  (C) 2023 shachi
 *                 All rights reserved.
 *
 *      Filename:  ./src/main.c
 *   Description:  server 主流程
 *
 *       Version:  0.1.0
 *        Author:  shachi <shachi1758@outlook.com>
 *     ChangeLog:  1. 2023-03-29 11:45:04 创建文件
 *
 *****************************************************************************/
#include <sqlite3.h>
#include <stdlib.h>
#include "server.h"
#include "klogger.h"
#include "database.h"


int main(int argc, const char* argv[])
{
    KINFO("服务启动\n");

    // 执行 sql 语句，建用户表
    char *sql = "CREATE TABLE IF NOT EXISTS USER(\
                 uid INTEGER PRIMARY KEY AUTOINCREMENT,\
                 uname TEXT NOT NULL,\
                 upsw TEXT NOT NULL,\
                 ustate INTEGER NOT NULL,\
                 usfd INTEGER NOT NULL);";
    create_tb(sql);
    // 建立 好友表
    sql = "CREATE TABLE IF NOT EXISTS FRIENDSHIP(\
          uid1 INTEGER NOT NULL,\
          uid2 INTEGER NOT NULL,\
          PRIMARY KEY (uid1,uid2));";
    create_tb(sql);

    // create group table
    sql = "CREATE TABLE IF NOT EXISTS GROUPS(\
           gid INTEGER PRIMARY KEY AUTOINCREMENT,\
           gname TEXT NOT NULL,\
           guid INTEGER NOT NULL,\
           gsum INTEGER NOT NULL);";
    create_tb(sql);
    // create group-user table
    sql = "CREATE TABLE IF NOT EXISTS GROUP_USER(\
           gid INTEGER NOT NULL,\
           guid INTEGER NOT NULL,\
           PRIMARY KEY (gid, guid));";
    create_tb(sql);

    return run();
}
