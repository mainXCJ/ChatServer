// @file: chatserver.cpp
// @author: 作者名称
// @date: 2023-xx-xx
// @brief: 聊天服务器主类实现，基于Muduo网络库构建，负责处理客户端的连接和消息转发

#include "chatserver.hpp"
#include "json.hpp"
#include "chatservice.hpp"

#include <iostream>
#include <functional>
#include <string>

using namespace std;
using namespace placeholders;
using json = nlohmann::json;

/**
 * @brief ChatServer类构造函数
 * @param loop 事件循环指针，Muduo库的核心，负责事件的分发和处理
 * @param listenAddr 服务器监听地址，包含IP和端口信息
 * @param nameArg 服务器名称，用于标识服务器实例
 */
ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop)
{
    // 注册连接回调函数，当客户端连接或断开时触发
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

    // 注册消息回调函数，当客户端发送消息时触发
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

    // 设置服务器的线程数量，这里使用4个IO线程
    _server.setThreadNum(4);
}

/**
 * @brief 启动聊天服务器
 * @note 调用此方法后，服务器开始监听指定地址和端口，并处理客户端连接
 */
void ChatServer::start()
{
    _server.start();
}

/**
 * @brief 连接事件回调函数
 * @param conn 连接指针，指向客户端连接对象
 * @note 当客户端连接或断开连接时，Muduo库会调用此函数
 */
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    // 检查客户端是否断开连接
    if (!conn->connected())
    {
        // 客户端异常断开连接时的处理逻辑
        ChatService::instance()->clientCloseException(conn);
        
        // 关闭连接资源
        conn->shutdown();
    }
}

/**
 * @brief 消息事件回调函数
 * @param conn 连接指针，指向客户端连接对象
 * @param buffer 缓冲区指针，存储接收到的原始数据
 * @param time 时间戳，记录消息接收的时间
 * @note 当客户端发送消息时，Muduo库会调用此函数处理消息
 */
void ChatServer::onMessage(const TcpConnectionPtr &conn,
                           Buffer *buffer,
                           Timestamp time)
{
    // 从缓冲区中读取所有数据并转换为字符串
    string buf = buffer->retrieveAllAsString();

    // 测试代码：打印接收到的JSON格式数据
    cout << buf << endl;

    try
    {
        // 将JSON字符串解析为json对象
        json js = json::parse(buf);
        
        // 根据消息类型ID(msgid)获取对应的业务处理函数
        // 这里使用了工厂模式和单例模式，实现了网络层与业务层的解耦
        auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());
        
        // 调用对应的业务处理函数，处理具体的业务逻辑
        msgHandler(conn, js, time);
    }
    catch (const exception &e)
    {
        // 处理JSON解析异常
        cerr << "JSON parse error: " << e.what() << endl;
        
        // 可以在这里添加异常处理逻辑，例如向客户端返回错误信息
    }
}