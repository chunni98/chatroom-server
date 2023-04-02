/******************************************************************************
 *     Copyright:  (C) 2023 shachi
 *                 All rights reserved.
 *
 *      Filename:  ./incl/group.h
 *   Description:  This file
 *
 *       Version:  0.1.0
 *        Author:  shachi <shachi1758@outlook.com>
 *     ChangeLog:  1. 2023-04-01 16:37:47 创建文件
 *
 *****************************************************************************/
#ifndef _GROUP_H
#define _GROUP_H

#include <inttypes.h>

struct group{
    int32_t gid;
    int32_t uid;
    int32_t gsum;
    char gname[20];
};

#endif //_GROUP_H
