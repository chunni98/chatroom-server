-- 创建用户表
CREATE TABLE IF NOT EXISTS USER(
    -- uid 用户唯一标识符主键，非空
    uid INTEGER PRIMARY KEY NOT NULL,
    -- uname 用户名，非空
    uname TEXT NOT NULL,
    -- upsw 用户密码，非空
    upsw TEXT NOT NULL,
    -- ustate 登录状态，非空
    ustate INTEGER NOT NULL,
    -- usfd 客户端连接套接字，非空
    usfd INTEGER NOT NULL
    );

-- 插入数据

INSERT INTO USER(uid,uname,upsw,ustate,usfd)
VALUES(1,'张三','123',0,3);
