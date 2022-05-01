#include "groupModel.h"
#include "chatMysql.h"
#include "public.h"

#include <string.h>
#include "json.hpp"
using json = nlohmann::json;

namespace chatRoom
{

    bool GroupModel::createGroup(Group& group)
    {
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "insert into AllGroup(groupname, groupdesc) value('%s', '%s')", 
                group.getGroupName().c_str(), group.getGroupDesc().c_str());

        ChatMysql mysqlConn_;
        if(mysqlConn_.connect())
        {
            if(mysqlConn_.update(buf))
            {
                group.setGroupId(mysql_insert_id(mysqlConn_.getConnection()));
                return true;
            }
        }
        return false;
    }

    bool GroupModel::deleteGroup(int userId, int groupId)
    {
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "delete from AllGroup where id = '%d'", 
                groupId);

        ChatMysql mysqlConn_;
        if(mysqlConn_.connect())
        {
            if(mysqlConn_.update(buf))
            {
                memset(buf, 0, sizeof(buf));
                snprintf(buf, sizeof(buf), "delete from GroupUser where groupid = '%d'", 
                        groupId);
                return true;
            }
        }
        return false;
    }

    bool GroupModel::addGroup(int groupId, int userId, std::string role)
    {
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "insert into GroupUser(groupid, userid, grouproole) \
                value(%d, %d, '%s')", groupId, userId, role.c_str());   

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

    std::vector<Group> GroupModel::getUserAllGroup(int userId)
    {
        std::vector<Group> groupVec;

        char buf[1024];
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "select lg.id, lg.groupname, lg.groupdesc \
                from GroupUser as gu inner join AllGroup as lg on gu.groupid = lg.id \
                where gu.userid = '%d'", userId);

        ChatMysql mysqlConn_;
        if(mysqlConn_.connect())
        {
            MYSQL_RES* curQueryRes = nullptr;
            curQueryRes = mysqlConn_.query(buf);
            if(curQueryRes)
            {
                MYSQL_ROW row = nullptr;
                while((row = mysql_fetch_row(curQueryRes)) != nullptr)
                {
                    groupVec.emplace_back(atoi(row[0]), row[1], row[2]);
                }
            }
        }

        for(auto &group: groupVec)
        {
            char buf[1024];
            memset(buf, 0, sizeof(buf));
            snprintf(buf, sizeof(buf), "select u.id, u.name, u.state, gu.grouproole \
                    from User as u inner join GroupUser as gu on gu.userid = u.id \
                    where gu.groupid = '%d'", group.getGroupId());

            MYSQL_RES* curQueryRes = nullptr;
            curQueryRes = mysqlConn_.query(buf);
            if(curQueryRes)
            {
                MYSQL_ROW row = nullptr;
                while((row = mysql_fetch_row(curQueryRes)) != nullptr)
                {
                    GroupUser groupUser;
                    groupUser.setId(atoi(row[0]));
                    groupUser.setName(row[1]);
                    groupUser.setState(row[2]);
                    groupUser.setGroupRole(row[3]);
                    group.setGroupUser(groupUser);
                }
            }            
        }
        return groupVec;
    }

    Group GroupModel::getGroup(int groupId)
    {
        Group group;

        char buf[1024];
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "select groupname, groupdesc from AllGroup where \
                id = '%d'", groupId);

        ChatMysql mysqlConn_;
        MYSQL_RES* curQueryRes = nullptr;
        if(mysqlConn_.connect())
        {
            curQueryRes = mysqlConn_.query(buf);
            if(curQueryRes)
            {
                MYSQL_ROW row = nullptr;
                while((row = mysql_fetch_row(curQueryRes)) != nullptr)
                {
                    group.setGroupId(groupId);
                    group.setGroupName(row[0]);
                    group.setGroupDesc(row[1]);
                }
            }
        }          

        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "select u.id, u.name, u.state, gu.grouproole \
                from User as u inner join GroupUser as gu on gu.userid = u.id \
                where gu.groupid = '%d'", groupId);

        curQueryRes = nullptr;
        curQueryRes = mysqlConn_.query(buf);
        if(curQueryRes)
        {
            MYSQL_ROW row = nullptr;
            while((row = mysql_fetch_row(curQueryRes)) != nullptr)
            {
                GroupUser groupUser;
                groupUser.setId(atoi(row[0]));
                groupUser.setName(row[1]);
                groupUser.setState(row[2]);
                groupUser.setGroupRole(row[3]);
                group.setGroupUser(groupUser);
            }
        }
        return group;        
    }

  
} // namespace chatRoom