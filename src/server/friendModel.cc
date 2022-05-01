#include "friendModel.h"
#include "chatMysql.h"
#include <string.h>


namespace chatRoom
{

    bool FriendModel::addFriend(int localId, int peerId)
    {
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "insert into Friend(userid, friendid) values('%d', '%d')",
                        localId, peerId);

        ChatMysql mysqlConn_;
        if(mysqlConn_.connect())
        {
            if(mysqlConn_.update(buf))
            {
                return true;
            }
        }
        return false;
    }

    bool FriendModel::deleteFriend(int localId, int peerId)
    {
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "delete from Friend where userid = '%d' and friendid = '%d'",
                        localId, peerId);
        ChatMysql mysqlConn_;
        if(mysqlConn_.connect())
        {
            if(mysqlConn_.update(buf))
            {
                return true;
            }
        }
        return false;       
    }

    std::vector<User> FriendModel::FriendList(int localId)
    {
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "select u.id,u.name, u.state from User as u inner join Friend as f on  f.friendid = u.id where f.userid='%d'", 
                        localId);

        std::vector<User> UserFriendList;
    
        ChatMysql mysqlConn_;
        if(mysqlConn_.connect())
        {
            MYSQL_RES* curQueryRes = mysqlConn_.query(buf);
            if(curQueryRes != nullptr)
            {
                MYSQL_ROW row = nullptr;
                while((row = mysql_fetch_row(curQueryRes)) != nullptr)
                {
                    UserFriendList.emplace_back(atoi(row[0]), row[1], row[2]);
                }
            }
        }
        return UserFriendList;

    }

    
} // namespace chatRoom
