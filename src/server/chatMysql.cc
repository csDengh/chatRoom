#include "chatMysql.h"
#include "mysqlConnPool.h"

#include <muduo/base/Logging.h>


namespace chatRoom
{

    ChatMysql::ChatMysql()
    : resentRes_(nullptr)
    {
        conn_ = MysqlConnPool::getInstance()->getConn();
    }

    ChatMysql::~ChatMysql()
    {
        if(conn_ != nullptr)
            MysqlConnPool::getInstance()->releaseConn(conn_);
        if(resentRes_ != nullptr)
            mysql_free_result(resentRes_);
    }

    bool ChatMysql::connect()
    {
        return true;
    }

    bool ChatMysql::update(std::string sql)
    {
        int ret = mysql_query(conn_, sql.c_str());
        if(ret == 0)
        {
            return true;
        }
        else
        {
            LOG_INFO << __FILE__ << ":" << __LINE__ <<":"<<sql <<"更新失败"<< " error:" << mysql_error(conn_);
            return false;
        }
    }

    MYSQL_RES* ChatMysql::query(std::string sql)
    {
        int ret = mysql_query(conn_, sql.c_str());

        if(ret == 0)
        {
            if(resentRes_ != nullptr)
                mysql_free_result(resentRes_);
            resentRes_= mysql_use_result(conn_);
            return resentRes_;
        }
        else
        {
            LOG_INFO<< __FILE__ <<":" <<__LINE__<<":"<<sql<<"查询失败"<< " error:" << mysql_error(conn_);
            return nullptr;
        }
    }

    MYSQL* ChatMysql::getConnection()
    {
        return conn_;
    }


}; // namespace chatRoom
