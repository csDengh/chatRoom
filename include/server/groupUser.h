#pragma once

#include "user.h"

namespace chatRoom
{
    /**
     * @brief 群聊用户封装类
     */
    class GroupUser: public User
    {
    public:
        void setGroupRole(std::string groupRole) { groupRole_ = groupRole; }
        std::string getGroupRole() { return groupRole_; }
    private:
        /// 用户在群聊的角色
        std::string groupRole_;
    };
} // namespace chatRoom



