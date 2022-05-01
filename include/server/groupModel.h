#pragma once

#include "group.h"


namespace chatRoom
{
    /**
     * @brief group和mysql交互类
     */
    class GroupModel
    {
    public:
        static GroupModel* getGroupModelInstance()
        {
            static GroupModel groupModel_;
            return &groupModel_;
        }

        /**
         * @brief 创建一个group
         */
        bool createGroup(Group& group);

        /**
         * @brief 删除一个group
         */
        bool deleteGroup(int userId, int groupId);

        /**
         * @brief 用户id为userId加入到群聊id为groupId的群聊
         */
        bool addGroup(int userId, int groupId, std::string role);

        /**
         * @brief 获得用户的所以群聊
         */
        std::vector<Group> getUserAllGroup(int userId);
    
        /**
         * @brief 返回群聊id为groupId的群聊信息
         */
        Group getGroup(int groupId);

    private:
        GroupModel() = default;
    };
  
} // namespace chatRoom




