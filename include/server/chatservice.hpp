// @file: chatservice.hpp
// @author: 作者名称
// @date: 2023-xx-xx
// @brief: 聊天服务器业务逻辑类头文件，采用单例模式设计，处理客户端请求的业务逻辑

// 防止头文件重复包含的预处理指令
#ifndef CHATSERVICE_H
#define CHATSERVICE_H

// 引入Muduo网络库的TcpConnection头文件，用于处理TCP连接
#include<muduo/net/TcpConnection.h>
// 引入unordered_map头文件，用于存储消息ID与处理函数的映射关系
#include<unordered_map>
// 引入functional头文件，用于定义函数对象类型
#include <functional>
#include <mutex>

// 使用标准命名空间
using namespace std;
// 使用muduo命名空间
using namespace muduo;
// 使用muduo::net命名空间
using namespace muduo::net;

// 引入用户数据模型头文件
#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"
// 引入JSON库头文件，用于数据的序列化和反序列化
#include "json.hpp"
#include "groupmodel.hpp"
#include "redis.hpp"

// 定义JSON类型别名，方便使用
using json = nlohmann::json;

/**
 * @brief 消息处理函数类型定义
 * @param conn TCP连接指针，指向客户端连接
 * @param js JSON对象，存储客户端发送的消息内容
 * @param time 时间戳，记录消息到达的时间
 * @note 这是一个函数对象类型，用于存储不同消息类型对应的处理函数
 */
using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp)>;

/**
 * @brief 聊天服务器业务逻辑类
 * @note 采用单例模式设计，负责处理所有客户端请求的业务逻辑
 */
class ChatService
{
public:
    /**
     * @brief 获取ChatService类的单例实例
     * @return 返回ChatService类的唯一实例指针
     * @note 线程安全的单例模式实现，确保全局只有一个ChatService实例
     */
    static ChatService *instance();
    
    /**
     * @brief 处理用户登录业务
     * @param conn TCP连接指针，指向发起登录请求的客户端
     * @param js JSON对象，存储登录请求的详细信息（如用户名、密码等）
     * @param time 时间戳，记录登录请求到达的时间
     * @note 验证用户身份，处理登录成功或失败的逻辑
     */
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);
    
    /**
     * @brief 处理用户注册业务
     * @param conn TCP连接指针，指向发起注册请求的客户端
     * @param js JSON对象，存储注册请求的详细信息（如用户名、密码等）
     * @param time 时间戳，记录注册请求到达的时间
     * @note 为新用户创建账号，处理注册成功或失败的逻辑
     */
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 一对一聊天业务
    void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time);


    void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time);

    void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);

    void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);

    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);

    
    // 处理注销业务
    void loginout(const TcpConnectionPtr &conn, json &js, Timestamp time);
    
    /**
     * @brief 根据消息ID获取对应的消息处理函数
     * @param msgid 消息类型ID，用于标识不同的消息类型
     * @return 返回对应的消息处理函数对象
     * @note 根据消息类型ID查找并返回对应的处理函数，实现消息的分发处理
     */
    MsgHandler getHandler(int msgid);

    void handleRedisSubscribeMessage(int channel, string message);

    //处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr &conn);

    // 服务器异常，业务重置方法
    void reset();

private:
    /**
     * @brief 构造函数
     * @note 私有构造函数，确保只能通过instance()方法获取类实例（单例模式）
     */
    ChatService();

    /**
     * @brief 消息ID与处理函数的映射表
     * @note 使用unordered_map存储消息类型ID和对应的处理函数，实现O(1)时间复杂度的查找
     */
    unordered_map<int, MsgHandler> _msgHandlerMap;


    // 存储在线用户的通信连接
    unordered_map<int, TcpConnectionPtr> _userConnMap;


    // 互斥锁，保护_userConnMap的并发访问
    mutex _connMutex;

    /**
     * @brief 用户数据模型对象
     * @note 用于与数据库交互，处理用户数据的增删改查操作
     */
    UserModel _userModel;

    OfflineMsgModel _offlineMsgModel;

    FriendModel _friendModel;

    GroupModel _groupModel;

    Redis _redis;
};

// 头文件结束标记
#endif