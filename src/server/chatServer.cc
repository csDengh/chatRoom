#include "chatServer.h"
#include "chatService.h"

#include <functional>
#include <iostream>
#include "json.hpp"
#include <iostream>

using json = nlohmann::json;


namespace chatRoom
{

    ChatServer::ChatServer(EventLoop* loop, InetAddress address, string nameArg)
        : loop_(loop),
        server_(loop_, address, nameArg)
    {
        server_.setConnectionCallback(
            std::bind(&ChatServer::ChatServerConnectionCallback, this, std::placeholders::_1)
        );

        server_.setMessageCallback(
            std::bind(&ChatServer::ChatServerMessageCallback, this, std::placeholders::_1, 
                        std::placeholders::_2, std::placeholders::_3)
        );
    }

    ChatServer::~ChatServer()
    {
    }

    void ChatServer::start()
    {
        server_.start();
    }

    void ChatServer::setThreadNum(int nums)
    {
        threadNums_ = nums;
        server_.setThreadNum(threadNums_);
    }


    void ChatServer::ChatServerConnectionCallback(const TcpConnectionPtr &conPtr)
    {
        if(!conPtr->connected())
        {
            ChatService::getInstance()->clientCloseException(conPtr);
            conPtr->shutdown();
        }
    }

    void ChatServer::ChatServerMessageCallback(const TcpConnectionPtr &con_ptr,  Buffer *buffer,
                            Timestamp time)
    {
        std::string buf = buffer->retrieveAllAsString();
        json js = json::parse(buf);
        

        int msgId = js["msgId"].get<int>();

        auto msgCallback = ChatService::getInstance()->getHandler(msgId);

        msgCallback(con_ptr, js, time);

    }
} // namespace chatRoom