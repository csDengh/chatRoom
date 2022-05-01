#pragma once

#include <list>
#include <mysql/mysql.h>
#include <semaphore.h>
#include <pthread.h>
#include <string>
#include <muduo/base/Logging.h>

namespace chatRoom
{
    /**
     * @brief mysql连接池，单例
     */
    class MysqlConnPool
    {
    public:

        /**
         * @brief 初始化连接池
         */
        void init(int connNums, std::string mysqlHost, std::string mysqlUser, std::string mysqlPassword, std::string mysqlDbName);

        /**
         * @brief 析构释放连接池
         */        
        ~MysqlConnPool();

        static MysqlConnPool* getInstance()
        {
            static MysqlConnPool mysqlConnPool_;
            return &mysqlConnPool_;
        }

        /**
         * @brief 从连接池中获取一个连接
         */
        MYSQL* getConn();

        /**
         * @brief 释放一个连接到连接池
         */        
        void releaseConn(MYSQL *conn);

    private:
        MysqlConnPool();
        /// 连接池初始数量
        int connNums_;
        /// 连接池空闲数量
        int freeNum_;
        /// list管理连接池
        std::list<MYSQL*> ConnPool_;
        pthread_mutex_t mutex_;
        sem_t sem_;
        std::string mysqlHost_;
        std::string mysqlUser_;
        std::string mysqlPassword_;
        std::string mysqlDbName_;
    };

} // namespace chatRoom
