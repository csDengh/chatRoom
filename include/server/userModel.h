#pragma once


namespace chatRoom
{
    class User;

    /**
     * @brief user和mysql交互类
     * @details usermodel负责封装sql和msysql交互
     */
    class UserModel
    {
    public:
        static UserModel* getInstance()
        {
            static UserModel userModel_;
            return &userModel_;
        }

        /**
         * @brief 插入一个用户
         */
        bool insert(User& );

        /**
         * @brief 根据id返回user类对象
         */
        User query(int );

        /**
         * @brief 根据user的state信息更新到mysql
         */
        bool updateState(User& );

       /**
         * @brief 将user的state信息重置为offine
         */
        bool resetState();

    private:
        UserModel() = default;
    };
}; // namespace chatRoom
