#pragma once
#include "nocopyable.h"

#include <string>
#include <mysql/mysql.h>


namespace chatRoom
{
    /**
     * @brief mysql连接类
     */
    class ChatMysql: nocopyable
    {
    public:
        /**
         * @brief 通过连接池获取一个连接
         */
        ChatMysql();

        /**
         * @brief 析构时，释放连接，放到连接池
         */
        ~ChatMysql();

        /**
         * @brief deprecated
         */        
        bool connect();

        /**
         * @brief 根据sql语句修改
         * @param[in] sql sql语句
         */
        bool update(std::string sql);

        /**
         * @brief 根据sql语句查询
         * @return 返回查询的结果
         */ 
        MYSQL_RES *query(std::string sql);

        /**
         * @brief 获取连接
         */   
        MYSQL* getConnection();

        /**
         * @brief deprecated
         */ 
    private:
        /// mysql连接
        MYSQL *conn_;
        /// mysql请求的回应
        MYSQL_RES * resentRes_;
    };
} // namespace chatRoom


