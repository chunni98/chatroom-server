/******************************************************************************
 *     Copyright:  (C) 2023 shachi
 *                 All rights reserved.
 *
 *      Filename:  ./lib/clietn.c
 *   Description:  This file
 *
 *       Version:  0.1.0
 *        Author:  shachi <shachi1758@outlook.com>
 *     ChangeLog:  1. 2023-03-29 16:10:30 创建文件
 *
 *****************************************************************************/
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

int main(int argc, const char* argv[])
{
    int sfd = socket(AF_INET, SOCK_STREAM,0);

    struct sockaddr_in server;//声明服务器的IP和端口号
    server.sin_family=AF_INET;//地址族  代表选择IPv4
    server.sin_port=htons(2302);//端口号 //因为有大小端序之分，所以要转换端序
    server.sin_addr.s_addr=inet_addr("192.168.90.103");//IP地址 因为网络二进制


    return EXIT_SUCCESS;
}
