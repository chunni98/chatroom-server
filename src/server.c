/******************************************************************************
 *     Copyright:  (C) 2023 shachi
 *                 All rights reserved.
 *
 *      Filename:  ./src/server.c
 *   Description:  主流程
 *
 *       Version:  0.1.0
 *        Author:  shachi <shachi1758@outlook.com>
 *     ChangeLog:  1. 2023-03-29 11:46:57 创建文件
 *
 *****************************************************************************/
#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <string.h>
#include <sqlite3.h>
#include "usr.h"
#include "server.h"
#include "klogger.h"
#include "database.h"
#include "json2data.h"
#include "chat.h"
#include "group.h"

// 端口号
#define PORT            2302
// 最大等待连接数
#define MAX_CLIENTS     8
// 缓冲区大小
#define BUFFER_SIZE     1024

#define LIST_SIZE       50

//#define DEBUG

static void *chat(void *arg);
#ifndef DEBUG
static bool login(struct usr *user);
#endif
static bool client_exit(struct usr *user);

static bool add_friend(int32_t uid1, int32_t uid2);

static bool del_friend(int32_t uid1, int32_t uid2);

/**
 *  Description:    程序的主要流程控制
 *       Author:    shachi
 */
int32_t run()
{
    // 创建流式套接字
    int32_t sfd = socket(AF_INET,SOCK_STREAM,0);
    if (sfd == -1) {
        perror("创建套接字失败！");
        exit(EXIT_FAILURE);
    }
    // 初始化地址、端口
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    //   saddr.sin_addr.s_addr = inet_addr("192.168.90.103");
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(PORT);
    // 绑定
    if (bind(sfd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
        perror("绑定地址/端口失败！");
        exit(EXIT_FAILURE);
    }
    // 设置套接字监听
    if (listen(sfd, MAX_CLIENTS) == -1) {
        perror("设置监听套接字失败！");
        exit(EXIT_FAILURE);
    }
    // 等待并接受客户端的连接请求。
    struct sockaddr_in caddr;

    // 每连接一个客户端，则创建一个线程处理
    while (true) {
        int32_t cfd = accept(sfd, (struct sockaddr*)&caddr, &((socklen_t){sizeof(caddr)}));
        if (cfd == -1) {
            perror("接收请求失败！");
            exit(EXIT_FAILURE);
        }
        kdebug("连接成功！\n");
        // 创建线程处理
        pthread_t tid;
        if (pthread_create(&tid, NULL, chat, &cfd) != 0) {
            perror("创建线程失败！");
            exit(EXIT_FAILURE);
        }

        // 设置线程为分离属性
        if (pthread_detach(tid) != 0) {
            perror("设置线程分离属性失败！");
            exit(EXIT_FAILURE);
        }
    }

    // 关闭套接字
    close(sfd);

    return EXIT_FAILURE;
}

/**
 *   Description: 客户端的请求处理
 *    Input Args: arg，客户端的连接套接字地址
 *        Author: shachi
 */
static void *chat(void *arg)
{
    int32_t cfd = *(int32_t *)arg;
    kdebug("cfd:%d\n",cfd);
    struct usr user;


#ifndef DEBUG
    while (true) {
        char buf1[BUFFER_SIZE];
        // 阻塞等待用户登录或者注册
        int32_t recv_ret = recv(cfd, buf1, sizeof(buf1),0);
        if (recv_ret == -1 || recv_ret == 0) {
            if (recv_ret == -1) perror("接收信息失败");
            //client_exit(&user);
            kdebug("client (unlogin) is quit\n");
            close(cfd);
            pthread_exit(NULL);
        }

        kdebug("client:%s\n",buf1);
        // buf1 解包

        // logon
        int32_t login_ret = get_login_info(buf1,&user);
        kdebug("login_info:%d\n",login_ret);
        if (login_ret == LOGON) {
            const char *ret;
            add_user(&user);
            ret = set_logon_result(&user);
            send(cfd, ret, strlen(ret) + 1,0);
            continue;
        } else
            // quit
            if (login_ret == QUIT) {
                kdebug("client (unlogin) is quit\n");
                //client_exit(&user);
                close(cfd);
                pthread_exit(NULL);
            } else if (login_ret == LOGIN) {
                const char *buf;
                //kdebug("login");

                // login 查询数据库比对密码
                if (login(&user)) {
                    // login successful
                    // 更新数据库信息，设为在线状态，更新套接字
                    user.sfd = cfd;
                    user.state = 1;
                    update_user_cfd_and_state(&user);

                    // 获取好友列表
                    struct usr friends[LIST_SIZE];
                    size_t len = friends_query(&user, friends);
                    kdebug("获取好友列表成功！\n");
                    buf = packjson(true,friends,len);
                    kdebug("sendjson:%s\n",buf);
                    send(cfd,buf,strlen(buf)+1,0);
                    // 发送群列表
                    break;
                } else {
                    // 登录失败！
                    buf = packjson(false,NULL,0);
                    kdebug("sendjson:%s\n",buf);
                    send(cfd,buf,strlen(buf)+1,0);
                    continue;
                }
            } else {
                kdebug("login_ret is %d\n",login_ret);
                //client_exit(&user);
                close(cfd);
                pthread_exit(NULL);
            }
    } // while
    //printf("exit\n");

    //pthread_exit(NULL);

#endif

    // login!
    // 处理请求

#ifdef DEBUG
    user.uid = 1;
    user.sfd = cfd;
    strcpy(user.name,"zhangsan");
    strcpy(user.psw,"psw");
    user.state = 1;

#endif
    const char *buf_ret = NULL;
    struct usr friends[LIST_SIZE];
    bool state = false;
    //kdebug("before while...,uid:%d,sfd:%d\n",user.uid,user.sfd);
    while (true) {
        struct usr dstuser;
        char buf[1024];
        int32_t recv_ret = recv(cfd,buf,sizeof(buf),0);
        if (recv_ret == -1 || recv_ret == 0) {
            goto EXIT_PTHREAD;
        }
        int ret = get_request(buf, &dstuser);
        KINFO("get_request,ret:%d\n",ret);


        switch (ret){
            case ADDFRIEND:
                {
                    state = add_friend(user.uid,dstuser.uid);
                    if (!state) {
                        kdebug("add false\n");
                        buf_ret = packjson(false,NULL,0);
                    }
                    else {
                        kdebug("add true\n");
                        size_t len = friends_query(&user, friends);
                        kdebug("获取好友列表成功！\n");
                        buf_ret = packjson(true,friends,len);
                    }
                    kdebug("send_result_json:%s\n",buf_ret);
                    send(cfd,buf_ret,strlen(buf_ret)+1,0);
                    break;
                }
            case DELFRIEND:
                {
                    state = del_friend(user.uid,dstuser.uid);
                    if (!state) {
                        kdebug("del false\n");
                        buf_ret = packjson(false,NULL,0);
                    }
                    else {
                        kdebug("del true\n");
                        size_t len = friends_query(&user, friends);
                        kdebug("获取好友列表成功！\n");
                        buf_ret = packjson(true,friends,len);
                    }
                    kdebug("send_result_json:%s\n",buf_ret);
                    send(cfd,buf_ret,strlen(buf_ret)+1,0);
                    break;
                }
            case SECRET_CHAT:
                secret_chat(&user,&dstuser);
                //buf_ret = packjson(false,NULL,0);
                break;
            case GROUP_CHAT:
                dstuser.sfd = user.sfd;
                group_chat(&dstuser);
                break;
            case QUIT:
                goto EXIT_PTHREAD;
                // get all friends's info
            case GET_FRIENDS:
                {

                    break;
                }
            case CREATE_GROUP:
                {
                    user.data = dstuser.data;
                    size_t gid = insert_group(&user);
                    if (gid != -1) {
                        buf_ret = set_fail();
                    } else {
                        buf_ret = set_successful();
                    }
                    // 发送
                    kdebug("send_result_json:%s\n",buf_ret);
                    send(cfd,buf_ret,strlen(buf_ret)+1,0);
                    break;
                }
            case GET_GROUP:
                {
                    // 获取结果
                    struct group groups[LIST_SIZE];
                    size_t num = groups_query(&user,groups);
                    // 结果打包
                    if (num == -1) {
                        buf_ret = groups_to_json(false,user.uid,groups,num);
                    } else {
                        buf_ret = groups_to_json(true,user.uid,groups,num);
                    }
                    // 发送
                    kdebug("send_result_json:%s\n",buf_ret);
                    send(cfd,buf_ret,strlen(buf_ret)+1,0);
                    break;
                }
            case GROUP_ADD:
                {
                    bool state = insert_group_user(dstuser.gid,dstuser.uid);
                    if (!state) {
                        buf_ret = set_fail();
                    } else {
                        buf_ret = set_successful();
                    }
                    // 发送
                    kdebug("send_result_json:%s\n",buf_ret);
                    send(cfd,buf_ret,strlen(buf_ret)+1,0);
                    break;
                }
            case GROUP_DEL:
                {
                    bool state = del_group_user(user.uid,dstuser.gid,dstuser.uid);
                    if (!state) {
                        buf_ret = set_fail();
                    } else {
                        buf_ret = set_successful();
                    }
                    // 发送
                    kdebug("send_result_json:%s\n",buf_ret);
                    send(cfd,buf_ret,strlen(buf_ret)+1,0);
                    break;

                    break;
                }
            default:
                goto EXIT_PTHREAD;
        }
    }
    // 退出线程
EXIT_PTHREAD:
    kdebug("%d is quit\n",user.uid);
    kdebug("%d closed\n",cfd);
    close(cfd);
    client_exit(&user);
    pthread_exit(NULL);
}

#ifndef DEBUG
// 登陆成功返回 true，用户不存在或者密码错误返回 false
static bool login(struct usr *user)
{
    sqlite3 *db;

    if (sqlite3_open(DATABASE_PATH,&db) != SQLITE_OK) {
        KERROR("无法打开数据库 %s\n", sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }
    struct usr user1;
    user1.uid = user->uid;
    if (!user_query(db,&user1)) return false;
    if (strcmp(user->psw,user1.psw) == 0) {
        return true;
    }
    sqlite3_close(db);

    return false;
}
#endif

static bool client_exit(struct usr *user)
{
    // pthread_create()
    user->state = 0;
    user->sfd = -1;
    if (!update_user_cfd_and_state(user)) return false;
    return true;
}

static bool add_friend(int32_t uid1, int32_t uid2)
{
    kdebug("%d,%d,add friend\n",uid1,uid2);
    sqlite3 *db;
    if (sqlite3_open(DATABASE_PATH,&db) != SQLITE_OK) {
        KERROR("无法打开数据库 %s\n", sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }
    // uid2 is exists
    struct usr usertmp;
    usertmp.uid = uid2;
    if (!user_query(db, &usertmp)) {
        kdebug("uid2 is not exists!\n");
        return false;
    }
    kdebug("uid1_uid2\n");
    // uid2 is not friend
    if (uid1_uid2(db,uid1,uid2) ) {
        kdebug("uid1 is uid2's friend\n");
        return false;
    }

    // add uid2
    kdebug("adding\n");
    if (!insert_friend(db, uid1, uid2)) {
        return false;
    }

    sqlite3_close(db);
    return true;
}

static bool del_friend(int32_t uid1, int32_t uid2)
{
    kdebug("delete friend\n");
    sqlite3 *db;
    if (sqlite3_open(DATABASE_PATH,&db) != SQLITE_OK) {
        KERROR("无法打开数据库 %s\n", sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }
    struct usr usertmp;
    usertmp.uid = uid2;
    if (!user_query(db, &usertmp)) {
        kdebug("uid2 is not exists!\n");
        return false;
    }
    // if uid1 is not uid2's friend
    if (!uid1_uid2(db,uid1,uid2)) {
        kdebug("uid1 is not uid2's friend!\n");
        return false;
    }
    if (!delete_friend(db, uid1, uid2)) {
        return false;
    }
    return true;
}
