#include "chatService.h"
#include "public.h"
#include "userModel.h"
#include "user.h"
#include "offlineMsgModel.h"
#include "friendModel.h"
#include "groupModel.h"
#include "chatRedis.h"
#include "mysqlConnPool.h"

#include <functional>
#include <muduo/base/Logging.h>
#include <muduo/net/TcpConnection.h>
#include <mutex>
#include <iostream>

using namespace muduo;
using json = nlohmann::json;

namespace chatRoom
{

    void ChatService::clientCloseException(const TcpConnectionPtr &connPtr)
    {
        User user_;
        {
            std::lock_guard<std::mutex> lock(connMutex_);
            for(auto it = userConnMap_.begin(); it != userConnMap_.end(); ++it)
            {
                if(it->second == connPtr)
                {
                    user_.setId(it->first);
                    userConnMap_.erase(it);
                    break;
                }
            }
        }
        

        if(user_.getId() != -1)
        {
            ChatRedis::getRedisInstance()->redisUnsubscribe(user_.getId());

            LOG_INFO<<"close";
            user_.setState("offline");
            UserModel::getInstance()->updateState(user_);
        }
    }


    ChatService::MsgHandler ChatService::getHandler(int msgId)
    {
        if(msgHandlerMap_.find(msgId) == msgHandlerMap_.end())
        {
            return [=](const TcpConnectionPtr &conn, json &js, Timestamp)
            {
                LOG_ERROR << "msgid:" << msgId << "No coresponding handler";
            };
        }
        else
            return msgHandlerMap_[msgId];
    }

    void ChatService::handleRedisSubscribeMessage(int userid, std::string msg)
    {
        json js = json::parse(msg);

        int peerId = js["localId"].get<int>();
        {
            std::lock_guard<std::mutex> lock(connMutex_);
            if(userConnMap_.find(peerId) != userConnMap_.end())
            {
                userConnMap_[peerId]->send(msg); 
                return;
            }
        }
        OfflineMsgModel::getOffineMsgInstance()->MsgStore(js);
    }

    void ChatService::reset()
    {
        UserModel::getInstance()->resetState();
    }

    ChatService::ChatService()
    {
        msgHandlerMap_[LOGIN] = std::bind(&ChatService::login, this, std::placeholders::_1, 
                                std::placeholders::_2, std::placeholders::_3);

        msgHandlerMap_[LOGOUT] = std::bind(&ChatService::loginOut, this, std::placeholders::_1, 
                                std::placeholders::_2, std::placeholders::_3);

        msgHandlerMap_[REGISTR] = std::bind(&ChatService::registr, this, std::placeholders::_1, 
                                std::placeholders::_2, std::placeholders::_3);

        msgHandlerMap_[ONE_ON_ONE_CHAT] = std::bind(&ChatService::oneOnOneChat, this, std::placeholders::_1, 
                                std::placeholders::_2, std::placeholders::_3);

        msgHandlerMap_[ADD_FRIEND] = std::bind(&ChatService::addFriend, this, std::placeholders::_1, 
                                std::placeholders::_2, std::placeholders::_3); 

        msgHandlerMap_[CREATE_GROUP] = std::bind(&ChatService::createGroup, this, std::placeholders::_1, 
                                std::placeholders::_2, std::placeholders::_3);  

        msgHandlerMap_[ADD_GROUP] = std::bind(&ChatService::addGroup, this, std::placeholders::_1, 
                                std::placeholders::_2, std::placeholders::_3);  

        msgHandlerMap_[GROUP_CHAT] = std::bind(&ChatService::groupChat, this, std::placeholders::_1, 
                                std::placeholders::_2, std::placeholders::_3); 
        msgHandlerMap_[DEL_FRIEND] = std::bind(&ChatService::deleteFriend, this, std::placeholders::_1, 
                                std::placeholders::_2, std::placeholders::_3);
        ChatRedis::getRedisInstance()->setRedisResponseHandler(
            std::bind(&ChatService::handleRedisSubscribeMessage, this, std::placeholders::_1, std::placeholders::_2)
        );
        if(!ChatRedis::getRedisInstance()->redisConnection())
        {
            exit(0);
        }
        MysqlConnPool::getInstance()->init(8, "localhost", "root", "775825", "chartServerDb");
    }

    void ChatService::login(const TcpConnectionPtr &connPtr, json &js, Timestamp timestamp)
    {
        int localId = js["localId"].get<int>();
        std::string password_ = js["password"];

        User queryUser =  UserModel::getInstance()->query(localId);

        if(queryUser.getId() == localId && queryUser.getPassword() == password_)
        {
            if(queryUser.getState() == "online")
            {
                json response;
                response["msgId"] = LOGIN_ACK;
                response["errno"] = -1;
                response["errmsg"] = "This account is online.";
                connPtr->send(response.dump());
            }
            else
            {
                queryUser.setState("online");

                UserModel::getInstance()->updateState(queryUser);

                {
                    std::lock_guard<std::mutex> lock(connMutex_);
                    userConnMap_[queryUser.getId()] = connPtr;
                }

                ChatRedis::getRedisInstance()->redisSubscribe(localId);

                json response;
                response["msgId"] = LOGIN_ACK;
                response["errno"] = 0;
                response["localId"] = queryUser.getId();
                response["name"] = queryUser.getName();


                std::vector<std::string> offlineMsgVector =  OfflineMsgModel::getOffineMsgInstance()->MsgQuery(localId);
                OfflineMsgModel::getOffineMsgInstance()->MsgDel(localId);
                if(!offlineMsgVector.empty())
                {
                    response["offlineMsg"] = offlineMsgVector;
                }

                std::vector<User> friendVec =  FriendModel::getFriendModelInstance()->FriendList(localId);

                if(!friendVec.empty())
                {
                    std::vector<std::string> friendStrVec;
                    for(User &user: friendVec)
                    {
                        json js;
                        js["peerId"] = user.getId();
                        js["name"] = user.getName();
                        js["state"] = user.getState();
                        friendStrVec.emplace_back(js.dump());
                    }
                    response["friendsLists"] = friendStrVec;
                }

                std::vector<Group> groupVec = GroupModel::getGroupModelInstance()->getUserAllGroup(localId);

                if(!groupVec.empty())
                {
                    std::vector<std::string> strGroupVec;
                    for(auto &group: groupVec)
                    {
                        json jsGroup;
                        jsGroup["groupId"] = group.getGroupId();
                        jsGroup["groupName"] = group.getGroupName();
                        jsGroup["groupDesc"] = group.getGroupDesc();

                        std::vector<std::string> strGroupUserVec;
                        for(auto &groupUser: group.getGroupUserList())
                        {
                            json jsGroupUser;
                            jsGroupUser["localId"] = groupUser.getId();
                            jsGroupUser["name"] = groupUser.getName();
                            jsGroupUser["state"]  = groupUser.getState();
                            jsGroupUser["groupRole"] = groupUser.getGroupRole();
                            strGroupUserVec.emplace_back(jsGroupUser.dump());
                        }
                        jsGroup["GroupUser"] = strGroupUserVec;
                        strGroupVec.emplace_back(jsGroup.dump());
                    }
                    response["GroupUserLists"] = strGroupVec;
                }
                connPtr->send(response.dump());
            }

        }
        else
        {
            json response;
            response["msgId"] = LOGIN_ACK;
            response["errno"] = -2;
            response["errmsg"] = "Id or password is invalid.";

            connPtr->send(response.dump());
        }

    }

    void ChatService::registr(const TcpConnectionPtr &connPtr, json &js, Timestamp timestamp)
    {
        std::string name_ = js["name"];
        std::string password_ = js["password"];
        User user(-1, name_, password_);
        int ret =  UserModel::getInstance()->insert(user);

        json response;
        response["msgId"] = REGISTR_ACK;
        if( ret )
        {
            response["errno"] = 0;
            response["localId"] = user.getId();
        }
        else
        {
            response["errno"] = -1;
        }
        connPtr->send(response.dump());
    }

    void ChatService::oneOnOneChat(const TcpConnectionPtr &connPtr, json &js, Timestamp timestamp)
    {
        int peerId = js["peerId"].get<int>();
        
        json response;
        response["msgId"] = ONE_ON_ONE_CHAT_ACK;
        response["peerId"] = js["localId"].get<int>();
        response["localId"] = peerId;
        response["peerMsg"] = js["peerMsg"].get<std::string>();
        response["MsgSendTime"] = js["MsgSendTime"].get<std::string>();

        {
            std::lock_guard<std::mutex> lock(connMutex_);
            if(userConnMap_.find(peerId) != userConnMap_.end())
            {
                userConnMap_[peerId]->send(response.dump()); 
                return;
            }
        }

        User PeerUser;
        PeerUser = UserModel::getInstance()->query(peerId);

        if(PeerUser.getId() != -1 && PeerUser.getState() == "online")
        {
            ChatRedis::getRedisInstance()->redisPublish(peerId, response.dump());
            return;
        }

        OfflineMsgModel::getOffineMsgInstance()->MsgStore(response);
    }

    void ChatService::addFriend(const TcpConnectionPtr &connPtr, json &js, Timestamp timestamp)
    {
        int localId = js["localId"].get<int>();
        int peerId = js["peerId"].get<int>();

        json response;
        char buf[40];
        memset(buf, 0, sizeof(buf));

        User peerUesr =  UserModel::getInstance()->query(peerId);

        if(peerUesr.getId() == peerId)
        {
            bool ret = FriendModel::getFriendModelInstance()->addFriend(localId, peerId);

            if(ret)
                snprintf(buf, sizeof(buf), "add friend[%d] success", peerId),
                response["errno"] = 0;
            else
                snprintf(buf, sizeof(buf), "add friend[%d] falied", peerId),
                response["errno"] = -1;
        }
        else
        {
            snprintf(buf, sizeof(buf), "no peerId[%d]", peerId);
            response["errno"] = -2;
        }
        response["msgId"] = ADD_FRIEND_ACK;
        response["AddFriendInfo"] = buf;
        connPtr->send(response.dump());
    }

    void ChatService::deleteFriend(const TcpConnectionPtr &connPtr, json &js, Timestamp timestamp)
    {
        int localId = js["localId"].get<int>();
        int peerId = js["peerId"].get<int>();

        json response;
        char buf[40];
        memset(buf, 0, sizeof(buf));

        bool ret = FriendModel::getFriendModelInstance()->deleteFriend(localId, peerId);
        if(ret)
            snprintf(buf, sizeof(buf), "delete friend[%d] success", peerId),
            response["errno"] = 0;

        else
            snprintf(buf, sizeof(buf), "delete friend[%d] falid", peerId),
            response["errno"] = -1;
        
        response["msgId"] = DEL_FRIEND_ACK;
        response["deleteFriendInfo"] = buf;

        connPtr->send(response.dump());
    }


    void ChatService::createGroup(const TcpConnectionPtr &connPtr, json &js, Timestamp timestamp)
    {
        std::string groupName = js["groupName"];
        std::string groupDesc = js["groupDesc"];
        Group group(-1, groupName, groupDesc);
        bool ret = GroupModel::getGroupModelInstance()->createGroup(group);

        json response;
        response["msgId"] = CREATE_GROUP_ACK;


        if(ret)
        {
            response["CreateGroupInfo"] = "create group successfully.";
            response["corespondingGroupId"] = group.getGroupId();
            response["errno"] = 0;
        }
        else
        {
            response["CreateGroupInfo"] = "fali to create group.";
            response["errno"] = -1;
        }
        connPtr->send(response.dump());
    }

    void ChatService::addGroup(const TcpConnectionPtr &connPtr, json &js, Timestamp timestamp)
    {
        int groupId = js["groupId"].get<int>();
        int userId = js["localId"].get<int>();
        std::string groupRole = js["groupRole"];
        
        bool ret = GroupModel::getGroupModelInstance()->addGroup(groupId, userId, groupRole);
        json response;
        response["msgId"] = ADD_GROUP_ACK;

        if(ret)
        {
            response["addGroupInfo"] = "add group successfully.";
            response["corespondingGroupId"] = groupId;
            response["errno"] = 0;
        }
        else
        {
            response["addGroupInfo"] = "fail to add group.";
            response["corespondingGroupId"] = groupId;
            response["errno"] = -1;
        }
        connPtr->send(response.dump());
    }

    void ChatService::groupChat(const TcpConnectionPtr &connPtr, json &js, Timestamp timestamp)
    {
        int groupId = js["groupId"];

        json response;
        response["msgId"] = GROUP_CHAT_ACK;
        response["peerId"] = js["localId"].get<int>();
        response["GroupMsg"] = js["GroupMsg"].get<std::string>();
        response["MsgSendTime"] = js["MsgSendTime"].get<std::string>();
        response["groupId"] = groupId;
        Group curGroup = GroupModel::getGroupModelInstance()->getGroup(groupId);
        
        std::lock_guard<std::mutex> lock(connMutex_);
        for(auto groupUser: curGroup.getGroupUserList())
        {
            response["localId"] = groupUser.getId();
            TcpConnectionPtr groupConnPtr = nullptr;

            if(userConnMap_.find(groupUser.getId()) != userConnMap_.end())
            {
                groupConnPtr = userConnMap_[groupUser.getId()];
                groupConnPtr->send(response.dump());
                continue;
            }

            if(groupUser.getState() == "online")
            {
                ChatRedis::getRedisInstance()->redisPublish(groupUser.getId(), response.dump());
                continue;
            }

            OfflineMsgModel::getOffineMsgInstance()->MsgStore(response);
        }
    }

    void ChatService::loginOut(const TcpConnectionPtr &connPtr, json &js, Timestamp timestamp)
    {
        int localId = js["localId"];
        {
            std::lock_guard<std::mutex> lock(connMutex_);
            userConnMap_.erase(localId);
        }

        ChatRedis::getRedisInstance()->redisUnsubscribe(localId);
        User user_(localId);
        LOG_INFO<<"close";
        user_.setState("offline");
        UserModel::getInstance()->updateState(user_);

    }



} // namespace chatRoom




