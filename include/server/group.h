#pragma once

#include "groupUser.h"

#include <vector>

namespace chatRoom
{
    /**
     * @brief 群聊封装类
     */
    class Group
    {
    public:
        Group(int groupId=-1, std::string groupName="", std::string groupDesc="")
        : groupId_(groupId),
        groupName_(groupName),
        groupDesc_(groupDesc)
        {
            
        }

        int getGroupId() { return groupId_; }
        std::string getGroupName() { return groupName_; }
        std::string getGroupDesc() { return groupDesc_; }
        std::vector<GroupUser> getGroupUserList() { return groupUserList_; }

        void setGroupId(int groupId) { groupId_ = groupId; }
        void setGroupName(std::string groupName) { groupName_ = groupName; }
        void setGroupDesc(std::string groupDesc) { groupDesc_ = groupDesc; }
        void setGroupUser(GroupUser& groupUser) { groupUserList_.emplace_back(groupUser); }

    private:
        /// 群聊id
        int groupId_;
        /// 群聊昵称
        std::string groupName_;
        /// 群聊描述
        std::string groupDesc_;
        /// 群聊的用户列表
        std::vector<GroupUser> groupUserList_;
    };
} // namespace chatRoom
