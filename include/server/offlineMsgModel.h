#pragma once

#include <vector>
#include "json.hpp"
using json = nlohmann::json;


namespace chatRoom
{
    /**
     * @brief user离线消息和msql交互类
     */
    class OfflineMsgModel
    {
    public:
        static OfflineMsgModel* getOffineMsgInstance()
        {
            static OfflineMsgModel offlineMsg;
            return &offlineMsg;
        }

        /**
         * @brief 根据userid返回user的离线消息
         */
        std::vector<std::string> MsgQuery(int id);

        /**
         * @brief 存储user的离线消息
         */
        bool MsgStore(json &js);

        /**
         * @brief 删除user离线消息
         */
        bool MsgDel(int id);
    private:
        OfflineMsgModel() = default;

    };
} // namespace chatRoom


