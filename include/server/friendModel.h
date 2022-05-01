#pragma once

#include "user.h"

#include <vector>


namespace chatRoom
{
    /**
     * @brief user好友信息和mysql交互类
     */
    class FriendModel
    {
    public:
        static FriendModel* getFriendModelInstance()
        {
            static FriendModel friendModel_;
            return &friendModel_;
        }

        /**
         * @brief localId添加一个peerId好友
         */
        bool addFriend(int localId, int peerId);

        /**
         * @brief localId删除一个peerId好友
         */
        bool deleteFriend(int localId, int peerId);

        /**
         * @brief 返回localId的好友列表
         */
        std::vector<User> FriendList(int localId);
    private:
        FriendModel() = default;
    };
    
} // namespace chatRoom



