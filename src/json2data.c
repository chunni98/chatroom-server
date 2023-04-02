/******************************************************************************
 *     Copyright:  (C) 2023 shachi
 *                 All rights reserved.
 *
 *      Filename:  ./src/json2data.c
 *   Description:  json 和 数据的转换
 *
 *       Version:  0.1.0
 *        Author:  shachi <shachi1758@outlook.com>
 *     ChangeLog:  1. 2023-03-29 14:40:37 创建文件
 *
 *****************************************************************************/

#include <json-c/json.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "usr.h"
#include "klogger.h"
#include "server.h"
#include "group.h"

int32_t get_login_info(const char arr[], struct usr *user)
{
    int32_t ret = LOGIN;
    // 字节流转 json 对象
    // 1. 根据 key 获取 json 对象
    struct json_object *obj = json_tokener_parse(arr);
    struct json_object *request_obj = json_object_object_get(obj, "request");
    struct json_object *uid_obj = json_object_object_get(obj, "uid");
    struct json_object *upsw_obj = json_object_object_get(obj, "upsw");
    struct json_object *uname_obj = json_object_object_get(obj, "uname");

    // 2. 从 json 对象中获取数据
    char request[20];
    strcpy(request,json_object_get_string(request_obj));
    // quit
    if (strcmp(request,"quit") == 0) {
        return QUIT;
    } else if (strcmp(request,"logon") == 0) {
        // 注册则获取名字
        strcpy(user->name,json_object_get_string(uname_obj));
        ret = LOGON;
    } else {
        // login
        user->uid = json_object_get_int(uid_obj);
    }
    strcpy(user->psw,json_object_get_string(upsw_obj));

    return ret;
}

// author: 刘佳辉
// 将要发给用户的数据进行json打包
const char * packjson(bool state,struct usr frends[],size_t len)
{
    struct json_object * objection = json_object_new_object();
    //创建json数组对象
    struct json_object * obj_arr = json_object_new_array();
    // state
    struct json_object *obj_state;

    if (!state) {
        obj_state = json_object_new_string("fail");
    } else {
        obj_state = json_object_new_string("successful");
    }

    json_object_object_add(objection,"state",obj_state);

    if (!state) {
        json_object_object_add(objection,"data",obj_arr);
        return json_object_to_json_string(objection);
    }


    int i =0;
    for(i = 0; i < len; i++){
        //将用户信息数据转化为json小对象
        struct json_object *obj_uid = json_object_new_int(frends[i].uid);
        struct json_object *obj_name = json_object_new_string(frends[i].name);
        struct json_object *obj_ustate = json_object_new_boolean(frends[i].state);
        //制作json大对象
        struct json_object * object = json_object_new_object();

        //将json小对象放入到json对象中，并贴上标签
        json_object_object_add(object,"uid",obj_uid);
        json_object_object_add(object,"uname",obj_name);
        json_object_object_add(object,"ustate",obj_ustate);

        //将当前json大对象放入数组对象中
        json_object_array_add(obj_arr,object);
    }

    json_object_object_add(objection,"data",obj_arr);
    //将json数组对象转化为字符串
    const char *p =json_object_to_json_string(objection);

    return p;
}

// login successful to client
const char *set_logon_result(struct usr *user)
{
    struct json_object *state_obj = json_object_new_string("successful");
    struct json_object *uid_obj = json_object_new_int(user->uid);

    struct json_object *object = json_object_new_object();

    json_object_object_add(object,"state",state_obj);
    json_object_object_add(object,"uid",uid_obj);

    return json_object_to_json_string(object);
}

// chat info return to client
const char *set_chat_info(struct usr *user1, struct usr *user2)
{
    struct json_object *state_obj = json_object_new_string("secret_chat");
    struct json_object *uid_obj = json_object_new_int(user1->uid);
    struct json_object *data_obj = json_object_new_string(user2->data);

    struct json_object *object = json_object_new_object();

    json_object_object_add(object,"state",state_obj);
    json_object_object_add(object,"uid",uid_obj);
    json_object_object_add(object,"data",data_obj);

    return json_object_to_json_string(object);
}

const char *set_group_chat_info(struct usr *user)
{
    struct json_object *state_obj = json_object_new_string("group_chat");
    struct json_object *gid_obj = json_object_new_int(user->gid);
    struct json_object *data_obj = json_object_new_string(user->data);

    struct json_object *object = json_object_new_object();

    json_object_object_add(object,"state",state_obj);
    json_object_object_add(object,"gid",gid_obj);
    json_object_object_add(object,"data",data_obj);

    return json_object_to_json_string(object);
}

const char *set_successful()
{
    return "{\"state\":\"successful\"}";
}
const char *set_fail()
{
    return "{\"state\":\"fail\"}";
}

int32_t get_request(const char arr[],struct usr *user)
{
    int32_t ret = 0;
    struct json_object *obj = json_tokener_parse(arr);
    struct json_object *request_obj = json_object_object_get(obj,"request");

    const char *request = json_object_get_string(request_obj);

    if (strcmp(request, "addfriend") == 0) {
        struct json_object *uid_obj = json_object_object_get(obj,"uid");
        user->uid = json_object_get_int(uid_obj);
        ret = ADDFRIEND;
    } else if (strcmp(request, "delfriend") == 0) {
        struct json_object *uid_obj = json_object_object_get(obj,"uid");
        user->uid = json_object_get_int(uid_obj);
        ret = DELFRIEND;
    } else if (strcmp(request, "secret_chat") == 0) {
        struct json_object *uid_obj = json_object_object_get(obj,"uid");
        user->uid = json_object_get_int(uid_obj);
        struct json_object *data_obj = json_object_object_get(obj,"data");
        //kdebug("%s\n",json_object_get_string(data_obj));
        //strcpy(user->data, json_object_get_string(data_obj));
        user->data = json_object_get_string(data_obj);
        ret = SECRET_CHAT;
    } else if (strcmp(request, "a_group") == 0) {
        // 获取群聊列表
        return GET_GROUP;
        // 创建群聊
    } else if (strcmp(request, "group") == 0) {
        struct json_object *name_obj = json_object_object_get(obj,"gname");
        user->data = json_object_get_string(name_obj);
        ret = CREATE_GROUP;
    } else if (strcmp(request, "group_chat") == 0) {
        // 群聊
        struct json_object *gid_obj = json_object_object_get(obj,"gid");
        user->gid = json_object_get_int(gid_obj);
        struct json_object *data_obj = json_object_object_get(obj,"data");
        user->data = json_object_get_string(data_obj);
        ret = GROUP_CHAT;
    } else if (strcmp(request, "group_add") == 0) {
        struct json_object *gid_obj = json_object_object_get(obj,"gid");
        user->gid = json_object_get_int(gid_obj);
        struct json_object *uid_obj = json_object_object_get(obj,"uid");
        user->uid = json_object_get_int(uid_obj);
        ret = GROUP_ADD;
    } else if (strcmp(request, "group_del") == 0) {
        struct json_object *gid_obj = json_object_object_get(obj,"gid");
        user->gid = json_object_get_int(gid_obj);
        struct json_object *uid_obj = json_object_object_get(obj,"uid");
        user->uid = json_object_get_int(uid_obj);
        ret = GROUP_DEL;
    }
    return ret;
}

const char * groups_to_json(bool state,int32_t uid, struct group groups[],size_t len)
{
    struct json_object * objection = json_object_new_object();
    //创建json数组对象
    struct json_object * obj_arr = json_object_new_array();
    // state
    struct json_object *obj_state;

    if (!state) {
        obj_state = json_object_new_string("fail");
    } else {
        obj_state = json_object_new_string("successful");
    }

    json_object_object_add(objection,"state",obj_state);

    if (!state) {
        json_object_object_add(objection,"data",obj_arr);
        return json_object_to_json_string(objection);
    }


    int i =0;
    for(i = 0; i < len; i++){
        //将用户信息数据转化为json小对象
        struct json_object *obj_gid = json_object_new_int(groups[i].gid);
        struct json_object *obj_gname = json_object_new_string(groups[i].gname);
        struct json_object *obj_is_owner;
        if (uid == groups[i].uid) {
            obj_is_owner = json_object_new_boolean(true);
        } else {
            obj_is_owner = json_object_new_boolean(false);
        }

        //制作json大对象
        struct json_object * object = json_object_new_object();

        //将json小对象放入到json对象中，并贴上标签
        json_object_object_add(object,"gid",obj_gid);
        json_object_object_add(object,"gname",obj_gname);
        json_object_object_add(object,"isOwner",obj_is_owner);

        //将当前json大对象放入数组对象中
        json_object_array_add(obj_arr,object);
    }

    json_object_object_add(objection,"data",obj_arr);
    //将json数组对象转化为字符串
    const char *p =json_object_to_json_string(objection);

    return p;
}
