#pragma once

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/noncopyable.h>

using namespace muduo::net;
using namespace muduo;


namespace chatRoom
{

    /**
     * @brief 服务器封装类
     */
    class ChatServer: noncopyable
    {

    public:
        ChatServer(EventLoop* loop, InetAddress address, string nameArg);
        ~ChatServer();

        /**
         * @brief 调用muduo的start
         */ 
        void start();

        /**
         * @brief 设置moduo的subreactor的数量
         */ 
        void setThreadNum(int nums);

    private:

        /**
         * @brief 连接建立或断开的事件处理函数
         */ 
        void ChatServerConnectionCallback(const TcpConnectionPtr &);

        /**
         * @brief 用户发送请求的可读事件处理函数
         * @details 将收到的信息反序列化为json，通过msgId字段获得对应的消息类型，通过消息类型返回对应的处理函数，然后执行对应的处理函数
         */ 
        void ChatServerMessageCallback(const TcpConnectionPtr &,  Buffer *,
                            Timestamp);
        /// muduo的eventloop
        EventLoop *loop_;
        /// muduo的tcpserver
        TcpServer server_;
        /// muduo的subreactor的数量
        int threadNums_;
    };
}; // namespace chatRoom
