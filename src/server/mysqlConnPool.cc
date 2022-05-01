#include "mysqlConnPool.h"


namespace chatRoom
{
    MysqlConnPool::MysqlConnPool()
    {
        pthread_mutex_init(&mutex_, nullptr);
        
    }

    void MysqlConnPool::init(int connNums, std::string mysqlHost, std::string mysqlUser, 
                                std::string mysqlPassword, std::string mysqlDbName)
    {
        connNums_ = connNums;
        mysqlHost_ = mysqlHost;
        mysqlUser_ = mysqlUser;
        mysqlPassword_ = mysqlPassword;
        mysqlDbName_ = mysqlDbName;
        connNums_ = connNums;
        freeNum_ = connNums;

        pthread_mutex_lock(&mutex_);
        for(int i=0; i<connNums_; ++i)
        {
            MYSQL *conn = mysql_init(nullptr);
            MYSQL* ret = mysql_real_connect(conn, mysqlHost_.c_str(), mysqlUser_.c_str(), 
                                mysqlPassword_.c_str(), mysqlDbName_.c_str(), 3306, nullptr, 0);
            if(ret != nullptr)
            {
                mysql_query(conn, "set names gbk");
                LOG_INFO<< "connect mysql success!";
            }
            else
            {
                LOG_INFO<< "connect mysql fail"<< " error:" << mysql_error(conn);
                exit(1);
            }
            ConnPool_.push_back(conn);
        }
        sem_init(&sem_, 0, connNums_);
        pthread_mutex_unlock(&mutex_);
    }

    MysqlConnPool::~MysqlConnPool()
    {
        pthread_mutex_lock(&mutex_);
        for(int i=0; i<ConnPool_.size(); ++i)
        {
            mysql_close(ConnPool_.front());
            ConnPool_.pop_front();
        }
        pthread_mutex_unlock(&mutex_);
	}

    MYSQL* MysqlConnPool::getConn()
    {
        sem_wait(&sem_);
        pthread_mutex_lock(&mutex_);

        MYSQL* conn_ = ConnPool_.front();
        ConnPool_.pop_front();
        --freeNum_;

        pthread_mutex_unlock(&mutex_);
        return conn_;
    }

    void MysqlConnPool::releaseConn(MYSQL *conn)
    {
        pthread_mutex_lock(&mutex_);
        ConnPool_.push_back(conn);
        ++freeNum_;
        pthread_mutex_unlock(&mutex_);
        sem_post(&sem_);
    }
} // namespace chatRoom
