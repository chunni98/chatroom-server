## 1 项目目标

### 1.1 基本功能

- 登录
- 注册
- 添加好友
- 好友私聊
- 创建群聊
- 群聊

### 1.2 扩展功能

- 删除好友
- 注销账号
- 显示好友信息
- 群拉人踢人
- vip 特权
- 发邮件等等

## 2 整体架构/模块划分

### 2.1 服务器

响应客户请求，处理数据

### 2.2 客户端

用户操作的

- 菜单界面：注册、登录、注销账号……

### 2.3 数据库

数据持久化

## 3 详细设计

### 3.1 数据库

**1. 用户**

用户(**账号**,密码,名字,套接字,在线状态)

**2. 聊天信息**

聊天信息(发送者,接收者,消息内容,时间戳)

好友表(**用户id**,好友id)

聊天组(**组号**,成员,管理员)