#pragma once

#include <functional>
#include <muduo/net/Callbacks.h>
#include <muduo/base/Timestamp.h>
#include <unordered_map>
#include <mutex>
#include "json.hpp"


using json = nlohmann::json;
using namespace muduo::net;
using namespace muduo;

namespace chatRoom
{
    /**
     * @brief 消息类型的处理类
     */
    class ChatService
    {
    public:
        using MsgHandler = std::function<void(const TcpConnectionPtr &, json &, Timestamp)>;
        
        static ChatService* getInstance()
        {   
            static ChatService chartServiceInstance;
            return &chartServiceInstance;
        }
        
        /**
         * @brief 用户close的处理函数
         */
        void clientCloseException(const TcpConnectionPtr &);
    
        /**
         * @brief 通过消息类型返回对应的处理函数
         */
        MsgHandler getHandler(int msgId);

        /**
         * @brief channel有消息时的回调处理函数
         */
        void handleRedisSubscribeMessage(int, string);

        /**
         * @brief 重置user状态
         */
        void reset();
    private:
        ChatService();

        /**
         * @brief 消息类型对应的处理函数
         */
        void login(const TcpConnectionPtr &, json &, Timestamp);
        void loginOut(const TcpConnectionPtr &, json &, Timestamp);
        void registr(const TcpConnectionPtr &, json &, Timestamp);
        void oneOnOneChat(const TcpConnectionPtr &, json &, Timestamp);
        void addFriend(const TcpConnectionPtr &, json &, Timestamp);
        void deleteFriend(const TcpConnectionPtr &, json &, Timestamp);
        void createGroup(const TcpConnectionPtr &, json &, Timestamp);
        void addGroup(const TcpConnectionPtr &, json &, Timestamp);
        void groupChat(const TcpConnectionPtr &, json &, Timestamp);

        /// 消息类型对应的处理函数的map msgId->MsgHandler
        std::unordered_map<int, MsgHandler> msgHandlerMap_;
        /// 连接建立的 userId-> TcpConnectionPtr的map
        std::unordered_map<int, TcpConnectionPtr> userConnMap_;
        std::mutex connMutex_;

    };
} // namespace chatRoom
