#pragma once

enum OperationType
{
    LOGIN = 1,
    LOGOUT,
    REGISTR,
    ADD_FRIEND,
    DEL_FRIEND,
    ONE_ON_ONE_CHAT,
    CREATE_GROUP,
    ADD_GROUP,
    GROUP_CHAT,

    LOGIN_ACK,
    REGISTR_ACK,
    ADD_FRIEND_ACK,
    DEL_FRIEND_ACK,
    ONE_ON_ONE_CHAT_ACK,
    CREATE_GROUP_ACK,
    ADD_GROUP_ACK,
    GROUP_CHAT_ACK
};

/* 
    response["peerId"] = js["localId"].get<int>();
    response["localId"] = peerId;
    response["peerMsg"] = js["peerMsg"];
    response["MsgSendTime"] = js["MsgSendTime"];
    
    ["peerId"]
    ["group"]
    js["groupName"];
    js["groupDesc"];

*/

// {"msgId":1, "localId":6, "password":"123456789"}
//  {"msgId":1, "localId":8, "password":"123456789"}
//   {"msgId":1, "localId":10, "password":"123456789"}
// {"msgId":4, "name":"ruby", "password":"123456789"}
// {"msgId":4, "name":"duby", "password":"123456789"}
// {"msgId":4, "name":"daby", "password":"123456789"}

//    {"msgId":7, "localId":6, "peerId":9}
       /// {"msgId":9, "localId":6, "peerId":9}

//{"msgId":6, "localId":8, "peerId":6, "peerMsg":"hello 7", "MsgSendTime":"20220411 11:33:45"}
// {"msgId":6, "localId":6, "peerId":8, "peerMsg":"hello xxxxxxxxxxxxxxxx", "MsgSendTime":"20220411 11:33:45"}
// {"msgId":11, "groupName": "group_1", "groupDesc":"test for group_1"}

// {"msgId":13, "groupId": 2, "userId":6, "groupRole": "normal"}
// {"msgId":13, "groupId": 2, "userId":8, "groupRole": "normal"}
// {"msgId":13, "groupId": 2, "userId":10, "groupRole": "normal"}

// {"msgId":9, "groupId": 2, "localId":6, "GroupMsg": "test for groupMsg", "MsgSendTime":"20220411 11:33:45"}


        // commandMap_["chatMenu"] = "显示所有支持的命令, 格式chatMenu";
        // commandMap_["oneOnOneChat"] = "一对一聊天, 格式oneOnOneChat:friendId:message";
        // commandMap_["addFriend"] = "添加好友, 格式addFriend:friendId";
        // commandMap_["createGroup"] = "创建群组, 格式createGroup:groupname:groupdesc";
        // commandMap_["addGroup"] = "加入群组, 格式addgroup:groupid";
        // commandMap_["groupChat"] = "群聊, 格式groupchat:groupid:message";
        // commandMap_["loginOut"] = "注销, 格式loginOut";