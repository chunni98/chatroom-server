/******************************************************************************
 *     Copyright:  (C) 2023 shachi
 *                 All rights reserved.
 *
 *      Filename:  ./incl/json2data.h
 *   Description:  json 与数据的转换
 *
 *       Version:  0.1.0
 *        Author:  shachi <shachi1758@outlook.com>
 *     ChangeLog:  1. 2023-03-29 14:08:54 创建文件
 *
 *****************************************************************************/
#ifndef _JSON_2_DATA_H
#define _JSON_2_DATA_H
#include <stdbool.h>
#include <stddef.h>
#include "usr.h"
#include "group.h"

/**
 *   Description: 从服务端传来的 json 字符串中获取数据，json 格式如下：
 *   {
 *    "request":"login",
 *     "uid":"1",
 *     "upsw":"222",
 *     "uname":null
 *   }
 *
 *    Input Args: arr，服务端传来的 json 字符串
 *   Output Args: user，存放解析出来数据的的数组。
 *  Return Value: 成功返回 true，失败返回 false
 */
int32_t get_login_info(const char arr[], struct usr *user);

const char *set_logon_result(struct usr *user);

const char * packjson(bool state,struct usr frends[],size_t len);

// 请求
int32_t get_request(const char arr[], struct usr *user);

const char *set_chat_info(struct usr *user1, struct usr *user2);


const char * groups_to_json(bool state,int32_t uid, struct group groups[],size_t len);
const char *set_successful();
const char *set_fail();
const char *set_group_chat_info(struct usr *user);
#endif // _JSON_2_DATA_H
