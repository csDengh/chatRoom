#pragma once

#include <hiredis/hiredis.h>
#include <functional>

namespace chatRoom
{
    /**
     * @brief redis封装类
     */
    class ChatRedis
    {
    public:

        static ChatRedis* getRedisInstance()
        {
            static ChatRedis chatRedis_("127.0.0.1", 6379);
            return  &chatRedis_;
        }

        ~ChatRedis();

        /**
         * @brief redis连接
         */
        bool redisConnection();

        /**
         * @brief 订阅频道channel
         */
        bool redisSubscribe(int channel);

        /**
         * @brief 取消订阅频道channel
         */
        bool redisUnsubscribe(int channel);

        /**
         * @brief 发布消息msg到频道channel
         */
        bool redisPublish(int localId, std::string msg);

        /**
         * @brief 线程的执行函数：对订阅channel的消息进行监听
         */
        void thread_msg_handle();

        /**
         * @brief 设置线程收到channel消息的处理函数
         */
        void setRedisResponseHandler(std::function<void(int, std::string)> handler);

    private:
        ChatRedis(std::string ip, int port);
        /// 发布的redis连接句柄
        redisContext* publishConn_;
        /// 订阅的redis连接句柄
        redisContext* subscribeConn_;
        /// 线程收到channel消息的处理函数
        std::function<void(int, std::string)> redisResponseHandle_;

        std::string ip_;
        int port_;

    };
} // namespace chatRoom


