#include "offlineMsgModel.h"
#include "chatMysql.h"


#include <string.h>
#include <vector>
#include <muduo/base/Logging.h>

namespace chatRoom
{
    std::vector<std::string> OfflineMsgModel::MsgQuery(int id)
    {
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "select message from OfflineMessage where userid = %d", id);

        ChatMysql chatMysqlConn_;

        std::vector<std::string> res;

        if(chatMysqlConn_.connect())
        {
            MYSQL_RES * queryRes = nullptr;
            queryRes = chatMysqlConn_.query(buf);
            if(queryRes)
            {
                MYSQL_ROW row = nullptr;
                while((row = mysql_fetch_row(queryRes)) != nullptr)
                {
                    res.push_back(row[0]);
                }
            }
        }
        return res;
    }

    bool OfflineMsgModel::MsgStore(json &js)
    {
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "insert into OfflineMessage(userid, message) values ('%d', '%s')", 
                    js["localId"].get<int>(), js.dump().c_str());
        
        ChatMysql chatMysqlConn_;

        if(chatMysqlConn_.connect())
        {
            bool ret = chatMysqlConn_.update(buf);
            return ret;
        }
        return false;
    }

    bool OfflineMsgModel::MsgDel(int id)
    {
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "delete from OfflineMessage where userid = %d", id); 
        ChatMysql chatMysqlConn_;
        if(chatMysqlConn_.connect())
        {
            bool ret = chatMysqlConn_.update(buf);
            return ret;
        }     
        return false;   

    }

} // namespace chatRoom
