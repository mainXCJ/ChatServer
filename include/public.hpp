#ifndef PUBLIC_H
#define PUBLIC_H

/*
server和client的公共文件
*/
enum EnMsgType
{
    LOGIN_MSG = 1,      // 登录消息
    LOGIN_MSG_ACK = 2,  // 登录响应消息
    REG_MSG = 3,        // 注册消息
    REG_MSG_ACK = 4,    // 注册响应消息
    ONE_CHAT_MSG = 5,   // 单聊消息
    ADD_FRIEND_MSG = 6, // 添加好友消息

    CREATE_GROUP_MSG, // 创建群组
    ADD_GROUP_MSG,    // 加入群组
    GROUP_CHAT_MSG,   // 群聊天
    LOGINOUT_MSG,// 
};

#endif