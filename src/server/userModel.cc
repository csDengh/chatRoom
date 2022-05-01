#include "userModel.h"
#include "user.h"
#include "chatMysql.h"

#include <string.h>


namespace chatRoom
{
    bool UserModel::insert(User& user)
    {
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "insert into User(name, password, state) values('%s', '%s', '%s')", user.getName().c_str(), user.getPassword().c_str(), user.getState().c_str());

        ChatMysql chatMysqlConn_;
        if(chatMysqlConn_.connect())
        {
            if(chatMysqlConn_.update(buf))
            {
                user.setId(mysql_insert_id(chatMysqlConn_.getConnection()));
                return true;
            }
        }
        return false;
    }

    User UserModel::query(int id)
    {
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "select * from User where id = %d", id);

        ChatMysql chatMysqlConn_;

        if(chatMysqlConn_.connect())
        {
            MYSQL_RES * queryRes = nullptr;
            queryRes = chatMysqlConn_.query(buf);
            if(queryRes)
            {
                MYSQL_ROW row = mysql_fetch_row(queryRes);
                if(row != nullptr)
                {
                    User user_;
                    user_.setId(atoi(row[0]));
                    user_.setName(row[1]);
                    user_.setPassword(row[2]);
                    user_.setState(row[3]);
                    return user_;
                }
            }
        }
        return User();
    }

    bool UserModel::updateState(User& user)
    {
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "update User set state = '%s' where id = %d", user.getState().c_str(), user.getId());

        ChatMysql chatMysqlConn_;

        if(chatMysqlConn_.connect())
        {
            bool ret = chatMysqlConn_.update(buf);
            if(ret)
            {
                return true;
            }
        }
        return false;
    }
    
    bool UserModel::resetState()
    {
        char sql[1024] = "update User set state = 'offline' where state = 'online'";

        ChatMysql mysql;
        if(mysql.connect())
        {
            bool ret = mysql.update(sql);
            if(ret)
            {
                return true;
            }
        }
        return false;
    }


}; // namespace chatRoom