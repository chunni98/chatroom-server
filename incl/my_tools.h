/******************************************************************************
 *     Copyright:  (C) 2023 shachi
 *                 All rights reserved.
 *
 *      Filename:  my_tools.h
 *   Description:  一些常用函数
 *
 *       Version:  0.1.0
 *        Author:  shachi <shachi1758@outlook.com>
 *     ChangeLog:  1. 2023-03-24 09:35:17 创建文件
 *
 *****************************************************************************/
#ifndef _SHACHI_MY_TOOLS_H
#define _SHACHI_MY_TOOLS_H

// 编译器在判断两个参数的地址是否相同的时候会先对其类型进行判断，如果相同则继续判断地址是否相同，否则编译器会给出一个警告
#define kmax(x, y) ({\
    typeof(x) _x = x;\
    typeof(y) _y = y;\
    (void)(&_x == &_y);\
    _x > _y ? _x : _y;})


#define kmin(x, y) ({\
    typeof(x) _x = x;\
    typeof(y) _y = y;\
    (void)(&_x == &_y);\
    _x < _y ? _x : _y;})

#endif // _SHACHI_MY_TOOLS_H
