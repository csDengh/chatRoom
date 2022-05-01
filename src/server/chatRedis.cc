#include "chatRedis.h"

#include <muduo/base/Logging.h>
#include <thread>

namespace chatRoom
{
    ChatRedis::ChatRedis(std::string ip, int port)
    :   ip_(ip),
    port_(port),
    publishConn_(nullptr),
    subscribeConn_(nullptr),
    redisResponseHandle_(nullptr)
    {
        
    }
    
    ChatRedis::~ChatRedis()
    {
        if(publishConn_ != nullptr) freeReplyObject(publishConn_);
        if(subscribeConn_ != nullptr) freeReplyObject(subscribeConn_);
    }

    bool ChatRedis::redisConnection()
    {
        publishConn_ = redisConnect(ip_.c_str(), port_);
        if(publishConn_ == nullptr)
        {
            LOG_INFO<<__FILE__ << __LINE__ <<"Redis Conn_ falied";
            return false;
        }

        subscribeConn_ = redisConnect(ip_.c_str(), port_);
        if(subscribeConn_ == nullptr)
        {
            LOG_INFO<<__FILE__ << __LINE__ <<"Redis Conn_ falied";
            return false;
        }

        std::thread responseHandlerThread(
            std::bind(&ChatRedis::thread_msg_handle, this)
        );
        responseHandlerThread.detach();

        LOG_INFO<<__FILE__ << __LINE__ <<"redis Connection success";
        return true;
    }

    //    redisAppendCommand->redisBufferWrite->redisGetReply
    bool ChatRedis::redisSubscribe(int channel)
    {
        if(REDIS_ERR == redisAppendCommand(subscribeConn_, "SUBSCRIBE %d", channel))
        {
            LOG_ERROR<<__FILE__ << __LINE__ <<"subscribe falied";
            return false;
        }

        int done = 0;
        while (!done)
        {
            if (REDIS_ERR == redisBufferWrite(subscribeConn_, &done))
            {
                LOG_ERROR<<__FILE__ << __LINE__ <<"subscribe falied";
                return false;
            }
        }
        LOG_INFO<<__FILE__ << __LINE__ <<"redis subbcribe success";
        return true;

    }

    bool ChatRedis::redisUnsubscribe(int channel)
    {
        if(REDIS_ERR == redisAppendCommand(subscribeConn_, "UNSUBSCRIBE %d", channel))
        {
            LOG_ERROR<<__FILE__ << __LINE__ <<"unsubscribe falied";
            return false;
        }

        int done = 0;
        while (!done)
        {
            if (REDIS_ERR == redisBufferWrite(subscribeConn_, &done))
            {
                LOG_ERROR<<__FILE__ << __LINE__ <<"unsubscribe falied";
                return false;
            }
        }
        LOG_INFO<<__FILE__ << __LINE__ <<"redis unsubbcribe success";
        return true;
    }

    bool ChatRedis::redisPublish(int localId, std::string msg)
    {
        redisReply *reply = (redisReply *)redisCommand(publishConn_, "PUBLISH %d %s", localId, msg.c_str());
        if (nullptr == reply)
        {
            LOG_ERROR<<__FILE__ << __LINE__ <<"publish command failed!";
            return false;
        }

        freeReplyObject(reply);
        LOG_INFO<<__FILE__ << __LINE__ <<"redis publish success";
        return true;
    }

    void ChatRedis::thread_msg_handle()
    {
        redisReply *reply = nullptr;
        while (REDIS_OK == redisGetReply(subscribeConn_, (void **)&reply))
        {
            if (reply != nullptr && reply->element[2] != nullptr && reply->element[2]->str != nullptr)
            {
                redisResponseHandle_(atoi(reply->element[1]->str) , reply->element[2]->str);
            }
            freeReplyObject(reply);
        } 

        LOG_ERROR << "thread redis response quit";
    }

    void ChatRedis::setRedisResponseHandler(std::function<void(int, std::string)> handler)
    {
        redisResponseHandle_ = handler;
    }

} // namespace chatRoom