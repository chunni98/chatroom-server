/******************************************************************************
 *     Copyright:  (C) 2023 shachi
 *                 All rights reserved.
 *
 *      Filename:  ./src/database.c
 *   Description:  定义数据库的操作
 *
 *       Version:  0.1.0
 *        Author:  shachi <shachi1758@outlook.com>
 *     ChangeLog:  1. 2023-03-29 16:31:12 创建文件
 *
 *****************************************************************************/
#include <stdbool.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "usr.h"
#include "klogger.h"
#include "database.h"

struct data {
    struct usr *user;
    size_t num;
};

struct group_data {
    struct group *groups;
    size_t num;
};

struct fd_data{
    int32_t *fds;
    size_t num;
};

/**
 *   Description: sqlite3_exec 的回调函数
 *    Input Args: data,sqlite3_exec() 的第四个参数
 *                argc,查询结果中的列数
 *                argv,指向当前查询结果集合的第一列指针
 *                colname,指向查询结果中第一列对应的列名的指针
 *  Return Value: 成功 返回 0，失败返回其他
 */
static int callback(void* data, int argc, char **argv, char **colname)
{
    struct data *dat = (struct data *)data;
    struct usr *user = dat->user;
    // 判断查询是否有结果
    if (argc == 0) {
        return 0;
    } else dat->num = 1;

    strcpy(user->name, argv[1]);
    strcpy(user->psw, argv[2]);
    //kdebug("%d\n",atoi(argv[4]);
    user->sfd = atoi(argv[4]);
    //kdebug("%d\n",user->sfd);
    //user->state = atoi(argv[3]);
    //user->sfd =  atoi(argv[4]);

    //printf("%s,%s,%s,%s\n",argv[1],argv[2],argv[3],argv[4]);

    return 0;
}

bool create_tb(char sql[])
{
    sqlite3 *db;

    if (sqlite3_open(DATABASE_PATH,&db) != SQLITE_OK) {
        KERROR("无法打开数据库 %s\n", sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    char *errmsg;
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK) {
        KERROR("执行 sql 错误：%s\n",errmsg);
        exit(EXIT_FAILURE);
    }
    sqlite3_close(db);

    return true;
}

bool user_query(sqlite3 *db, struct usr *user)
{
    // 构造 sql 语句
    char sql[50];
    sprintf(sql,"SELECT * FROM USER WHERE uid = %d;",user->uid);
    char *errmsg;
    struct data dat;
    dat.user = user;
    dat.num = 0;
    int32_t ret = sqlite3_exec(db, sql, callback, &dat, &errmsg);
    if (ret != SQLITE_OK) {
        KERROR("执行 sql 错误：%s\n",errmsg);
        exit(EXIT_FAILURE);
    }
    // 执行完成没有结果，代表用户不存在
    if (dat.num == 0) {
        return false;
    }


    return true;
}

/**
 *   Description: 把查询出来的好友列表标号添加到用户数组
 *    Input Args:
 *   Output Args: data，结构体数组
 *  Return Value:
 */
static int callback2(void* data, int argc, char **argv, char **colname)
{
    struct data *dat = (struct data *)data;
    struct usr *friends = dat->user;
    if (argc == 0) {
        // 查询没有结果
        dat->num = 0;
        return 0;
    }

    static size_t index = 0;
    if (dat->num == 0) index = 0;

    if (index >= 50) {
        KERROR("查询失败\n");
        exit(EXIT_FAILURE);
    }
    friends[index].uid = atoi(argv[argc - 1]);
    ++index;
    dat->num = index;

    return 0;
}

// 成功返回好友个数，失败返回 0
size_t friends_query(struct usr *user, struct usr friends[])
{
    sqlite3 *db;

    if (sqlite3_open(DATABASE_PATH,&db) != SQLITE_OK) {
        KERROR("无法打开数据库 %s\n", sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }
    char sql[50];
    sprintf(sql, "SELECT * FROM FRIENDSHIP WHERE uid1 = %d;", user->uid);
    char *errmsg;
    struct data dat;
    dat.user = friends;
    dat.num = 0;
    int32_t ret = sqlite3_exec(db, sql, callback2, &dat, &errmsg);
    if (ret != SQLITE_OK) {
        KERROR("执行 sql 错误：%s\n",errmsg);
        exit(EXIT_FAILURE);
    }
    // 执行完成没有结果，代表用户没有好友
    if (dat.num == 0) {
        sqlite3_close(db);
        return 0;
    }
    // 执行查找，获取朋友用户的信息。
    for (size_t i = 0; i < dat.num; ++ i){
        //        kdebug("%d ",dat.user[i].uid);
        if (!user_query(db,&dat.user[i])){
            sqlite3_close(db);
            return 0;
        }
        //        kdebug("%d,%s,%s",dat.user[i].uid,dat.user[i].name,dat.user[i].psw);
    }
    //   kdebug("\n");

    sqlite3_close(db);
    return dat.num;
}



// update cfd and state
bool update_user_cfd_and_state(struct usr *user)
{
    sqlite3 *db;
    if (sqlite3_open(DATABASE_PATH,&db) != SQLITE_OK) {
        KERROR("无法打开数据库 %s\n", sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }
    char sql[100];
    sprintf(sql, "UPDATE USER SET usfd = %d,ustate = %d WHERE uid == %d;",
            user->sfd,user->state,user->uid);
    kdebug("sql:%s\n",sql);

    char *errmsg;
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK) {
        KERROR("执行 sql 错误：%s\n",errmsg);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    return true;
}


static int callback3(void* data, int argc, char **argv, char **colname)
{
    int32_t *uid = (int32_t *)data;
    *uid = atoi(argv[0]);
    return 0;
}

bool add_user(struct usr *user)
{
    sqlite3 *db;
    if (sqlite3_open(DATABASE_PATH,&db) != SQLITE_OK) {
        KERROR("无法打开数据库 %s\n", sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }
    char sql[200];
    sprintf(sql, "INSERT INTO USER (uname, upsw,ustate,usfd) \
            VALUES ('%s','%s',%d,%d);",
            user->name,user->psw,0,0);
    kdebug("sql:%s\n",sql);

    char *errmsg;
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK) {
        KERROR("执行 sql 错误：%s\n",errmsg);
        sqlite3_close(db);
        return false;
    }

    // get uid
    strcpy(sql,"SELECT MAX(uid) FROM USER;");
    if (sqlite3_exec(db, sql, callback3, &user->uid, &errmsg) != SQLITE_OK) {
        KERROR("执行 sql 错误：%s\n",errmsg);
        sqlite3_close(db);
        return false;
    }


    sqlite3_close(db);
    return true;
}


static int callback4(void* data, int argc, char **argv, char **colname)
{
    if (argc > 0) *(bool *)data = true;
    return 0;
}

bool uid1_uid2(sqlite3 *db, int32_t uid1, int32_t uid2)
{
    char sql[200];
    sprintf(sql, "SELECT * FROM FRIENDSHIP WHERE uid1=%d AND uid2=%d;",
            uid1,uid2);
    char *errmsg;
    bool flag = false;
    if (sqlite3_exec(db, sql, callback4, &flag, &errmsg) != SQLITE_OK) {
        KERROR("执行 sql 错误：%s\n",errmsg);
        return false;
    }

    return flag;
}

bool insert_friend(sqlite3 *db, int32_t uid1, int32_t uid2)
{
    char sql[200];
    sprintf(sql, "INSERT INTO FRIENDSHIP (uid1,uid2)\
            VALUES (%d,%d),(%d,%d);",
            uid1,uid2,uid2,uid1);
    kdebug("sql:%s\n",sql);
    char *errmsg;
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK) {
        KERROR("执行 sql 错误：%s\n",errmsg);
        return false;
    }
    return true;
}

bool delete_friend(sqlite3 *db, int32_t uid1, int32_t uid2)
{
    char sql[200];
    sprintf(sql, "DELETE FROM FRIENDSHIP WHERE (uid1 = %d and uid2 = %d) or\
            (uid1 = %d and uid2 = %d);",
            uid1,uid2,uid2,uid1);
    kdebug("sql:%s\n",sql);
    char *errmsg;
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK) {
        KERROR("执行 sql 错误：%s\n",errmsg);
        return false;
    }

    return true;
}

static int callback5(void* data, int argc, char **argv, char **colname)
{
    struct group_data *dat = (struct group_data *)data;
    struct group *groups = dat->groups;

    // 判断查询是否有结果
    if (argc == 0) {
        dat->num = 0;
        return 0;
    };

    static size_t index = 0;
    if (dat->num == 0) index = 0;

    if (index >= 50) {
        KERROR("查询失败\n");
        return -1;
    }

    groups[index].gid = atoi(argv[0]);
    strcpy(groups[index].gname, argv[1]);
    groups[index].uid = atoi(argv[2]);

    ++index;
    dat->num = index;

    return 0;
}
size_t groups_query(struct usr *user, struct group groups[])
{

    sqlite3 *db;
    if (sqlite3_open(DATABASE_PATH,&db) != SQLITE_OK) {
        KERROR("无法打开数据库 %s\n", sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }
    char sql[200];
    sprintf(sql, "SELECT gid,gname,guid FROM GROUPS WHERE gid IN (SELECT gid \
        FROM GROUP_USER WHERE guid = %d);",
            user->uid);
    kdebug("sql:%s\n",sql);

    struct group_data dat;
    dat.num = 0;
    dat.groups = groups;

    char *errmsg;
    if (sqlite3_exec(db, sql, callback5, &dat, &errmsg) != SQLITE_OK) {
        KERROR("执行 sql 错误：%s\n",errmsg);
        sqlite3_close(db);
        return -1;
    }
    // 执行完成没有结果，代表用户没有群聊
    //if (dat.num == 0) {
    //    return 0;
    //}

    sqlite3_close(db);
    return dat.num;
}

static int callback6(void* data, int argc, char **argv, char **colname)
{
    int32_t *gid = (int32_t *)data;
    *gid = atoi(argv[0]);
    return 0;
}

int32_t insert_group(struct usr *user)
{
    int32_t gid;
    sqlite3 *db;
    if (sqlite3_open(DATABASE_PATH,&db) != SQLITE_OK) {
        KERROR("无法打开数据库 %s\n", sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }
    // 插入group 表
    char sql[200];
    sprintf(sql, "INSERT INTO GROUPS (gname, guid, gsum) VALUES('%s',%d,1);",
            user->data,user->uid);

    kdebug("sql:%s\n",sql);
    char *errmsg;
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK) {
        KERROR("执行 sql 错误：%s\n",errmsg);
        sqlite3_close(db);
        return -1;
    }

    sprintf(sql, "SELECT MAX(gid) FROM GROUPS;");
    if (sqlite3_exec(db, sql, callback6, &user->gid, &errmsg) != SQLITE_OK) {
        KERROR("执行 sql 错误：%s\n",errmsg);
        sqlite3_close(db);
        return -1;
    }

    kdebug("sql:%s\n",sql);
    sprintf(sql, "INSERT INTO GROUP_USER VALUES(%d,%d);",
            user->gid,user->uid);

    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK) {
        KERROR("执行 sql 错误：%s\n",errmsg);
        sqlite3_close(db);
        return -1;
    }

    kdebug("sql:%s\n",sql);

    sqlite3_close(db);
    return gid;
}


static int callback7(void* data, int argc, char **argv, char **colname)
{
    struct fd_data *dat = (struct fd_data *)data;
    int32_t *fds = dat->fds;

    if (argc == 0) {
        dat->num = 0;
        return 0;
    }

    static size_t index = 0;
    if (dat->num == 0) index = 0;

    if (index >= 50) {
        KERROR("查询失败\n");
        return -1;
    }

    fds[index] = atoi(argv[0]);
    ++index;
    dat->num = index;


    return 0;
}

// 返回在线人数，<= 0 代表没人在线，则不发送。
size_t get_online_user(int32_t gid, int32_t fds[])
{
    sqlite3 *db;
    if (sqlite3_open(DATABASE_PATH,&db) != SQLITE_OK) {
        KERROR("无法打开数据库 %s\n", sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }
    char sql[512];
    sprintf(sql,"SELECT usfd FROM USER WHERE usfd > -1 AND uid IN(SELECT guid \
        FROM GROUP_USER WHERE gid = %d);",gid);

    struct fd_data fdd;
    fdd.fds = fds;
    fdd.num = 0;

    char *errmsg;
    if (sqlite3_exec(db, sql, callback7, &fdd, &errmsg) != SQLITE_OK) {
        KERROR("执行 sql 错误：%s\n",errmsg);
        sqlite3_close(db);
        return -1;
    }

    kdebug("sql:%s\n",sql);


    sqlite3_close(db);
    return fdd.num;
}


static int callback8(void* data, int argc, char **argv, char **colname)
{
    if (argc > 0) {
        *(bool *)data = true;
    }
    return 0;
}

bool insert_group_user(int32_t gid, int32_t uid)
{
    sqlite3 *db;
    if (sqlite3_open(DATABASE_PATH,&db) != SQLITE_OK) {
        KERROR("无法打开数据库 %s\n", sqlite3_errmsg(db));
        return false;
    }
    char sql[256];
    char *errmsg;

    // gid is exists
    bool state = false;
    sprintf(sql,"select * from groups where gid = %d;",gid);
    if (sqlite3_exec(db, sql, callback8, &state, &errmsg) != SQLITE_OK) {
        KERROR("执行 sql 错误：%s\n",errmsg);
        sqlite3_close(db);
        return false;
    }
    kdebug("sql:%s\n",sql);
    if (!state) {
        sqlite3_close(db);
        return false;
    }

    // gsum add one
    sprintf(sql,"update groups set gsum = gsum +1 where guid = %d;",gid);
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK) {
        KERROR("执行 sql 错误：%s\n",errmsg);
        sqlite3_close(db);
        return false;
    }
    kdebug("sql:%s\n",sql);



    // insert
    sprintf(sql, "INSERT INTO GROUP_USER VALUES(%d,%d);", gid,uid);

    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK) {
        KERROR("执行 sql 错误：%s\n",errmsg);
        sqlite3_close(db);
        return false;
    }

    kdebug("sql:%s\n",sql);

    sqlite3_close(db);

    return true;
}
bool del_group_user(int32_t uida,int32_t gid, int32_t uidb)
{
    if (uida == uidb ) return false;
    sqlite3 *db;
    if (sqlite3_open(DATABASE_PATH,&db) != SQLITE_OK) {
        KERROR("无法打开数据库 %s\n", sqlite3_errmsg(db));
        return false;
    }
    char *errmsg;
    char sql[256];
    // query group's owner
    bool state = false;
    sprintf(sql,"select guid from groups where gid = %d and guid = %d;",gid, uida);
    if (sqlite3_exec(db, sql, callback8, &state, &errmsg) != SQLITE_OK) {
        KERROR("执行 sql 错误：%s\n",errmsg);
        sqlite3_close(db);
        return false;
    }
    kdebug("sql:%s\n",sql);
    if (!state) {
        sqlite3_close(db);
        return false;
    }

    // delete uidb
    sprintf(sql,"delete from group_user where gid = %d and guid = %d;",gid, uidb);
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK) {
        KERROR("执行 sql 错误：%s\n",errmsg);
        sqlite3_close(db);
        return false;
    }
    kdebug("sql:%s\n",sql);


    sqlite3_close(db);
    return true;
}
