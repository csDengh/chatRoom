#include "chatClient.h"
#include "public.h"

#include <chrono>
#include <unistd.h>
#include <iostream>
#include "json.hpp"
#include <ctime>
#include <thread>
#include <semaphore.h>


namespace chatRoom
{

    ChatClient::ChatClient(std::string ip, int port)
    :   loginState_(false),
    chatMenuState_(false),
    chatCommand_(),
    currentUserFriends_(),
    mainMenuLoopState_(false)
    {
        memset(&serverInetAddr_, 0, sizeof(serverInetAddr_));
        serverInetAddr_.sin_addr.s_addr = inet_addr(ip.c_str());
        serverInetAddr_.sin_port = htons(port);
        serverInetAddr_.sin_family = AF_INET;
        sem_init(&rwsem_, 0, 0);

        commandMap_["chatMenu"] = "显示所有支持的命令, 格式help";
        commandMap_["oneOnOneChat"] = "一对一聊天, 格式oneOnOneChat:friendid:message";
        commandMap_["addFriend"] = "添加好友, 格式addFriend:friendid";
        commandMap_["createGroup"] = "创建群组, 格式createGroup:groupname:groupdesc";
        commandMap_["addGroup"] = "加入群组, 格式addGroup:groupid";
        commandMap_["groupChat"] = "群聊, 格式groupChat:groupid:message";
        commandMap_["loginOut"] = "注销, 格式loginOut";

        commandHandlerMap_["chatMenu"] = std::bind(&ChatClient::chatMenu, this);
        commandHandlerMap_["oneOnOneChat"] = std::bind(&ChatClient::oneOnOneChat, this);
        commandHandlerMap_["addFriend"] = std::bind(&ChatClient::addFriend, this);
        commandHandlerMap_["createGroup"] = std::bind(&ChatClient::createGroup, this);
        commandHandlerMap_["addGroup"] = std::bind(&ChatClient::addGroup, this);
        commandHandlerMap_["groupChat"] = std::bind(&ChatClient::groupChat, this);
        commandHandlerMap_["loginOut"] = std::bind(&ChatClient::loginOut, this);
    }

    ChatClient::~ChatClient()
    {
        close(clientFd_);
        sem_destroy(&rwsem_);
    }


    void ChatClient::start()
    {
        chatConnect();
        mainMenu();
        mainMenuLoopState_ = true;

        std::thread responseThread(std::bind(&ChatClient::readTaskHandler, this));
        responseThread.detach();
    }

    void ChatClient::loop()
    {
        while(mainMenuLoopState_)
        {
            mainMenuChoice();
        }
    }

    void ChatClient::chatConnect()
    {
        clientFd_ = socket(AF_INET, SOCK_STREAM, 0);
        if(clientFd_ == -1)
        {
            std::cerr << "socket create error" << std::endl;
            exit(-1);
        }
        int ret = connect(clientFd_, (sockaddr*)&serverInetAddr_, sizeof(sockaddr_in));
        if(ret == -1)
        {
            std::cerr << " connect server error"<<std::endl;
            exit(-1);
        }
    }

    void ChatClient::mainMenu()
    {
        // 显示首页面菜单 登录、注册、退出
        std::cout << "========================" << std::endl;
        std::cout << "1. login" << std::endl;
        std::cout << "2. register" << std::endl;
        std::cout << "3. quit" << std::endl;
        std::cout << "========================" << std::endl;
        std::cout << "choice:";
    }

    void ChatClient::mainMenuChoice()
    {
        int choice = 1;
        std::cin >> choice;
        std::cin.get(); 
        switch (choice)
        {
            // login业务
            case 1: mainMenulogin(); break;
            // register业务
            case 2: mainMenuRegister(); break;
            // quit业务
            case 3: mainMenuQuit(); break;
            // error业务
            default:mainMenuError(); break;
        }
    }

    void ChatClient::mainMenulogin()
    {
        int localId = 1;
        char pwd[50] = {0};
        std::cout << "userid:";
        std::cin >> localId;
        std::cin.get();
        std::cout << "userpassword:";
        std::cin.getline(pwd, 50);
        

        json jsrequest;
        jsrequest["msgId"] = LOGIN;
        jsrequest["localId"] = localId;
        jsrequest["password"] = pwd;

        std::string request = jsrequest.dump();

        int len = send(clientFd_, request.c_str(), strlen(request.c_str()) + 1, 0);
        if (len == -1)
        {
            std::cerr << "send login msg error:" << request << std::endl;
        }

        sem_wait(&rwsem_); // 等待信号量，由子线程处理完登录的响应消息后，通知这里
    
        if (loginState_) 
        {
            // 进入聊天主菜单页面
            chatMenuState_ = true;
            chatLoop();
        }
    }

    void ChatClient::mainMenuRegister()
    {
        char name[50] = {0};
        char pwd[50] = {0};
        std::cout << "userName:";
        std::cin.getline(name, 50);
        std::cout << "userPassword:";
        std::cin.getline(pwd, 50);

        json js;
        js["msgId"] = REGISTR;
        js["name"] = name;
        js["password"] = pwd;
        std::string request = js.dump();

        int len = send(clientFd_, request.c_str(), strlen(request.c_str()) + 1, 0);
        if (len == -1)
        {
            std::cerr << "send reg msg error:" << request << std::endl;
        }
        
        sem_wait(&rwsem_); // 等待信号量，子线程处理完注册消息会通知
    }

    void ChatClient::mainMenuQuit()
    {
        mainMenuLoopState_ = false;
    }

    void ChatClient::mainMenuError()
    {
        std::cerr << "invalid input!" << std::endl;
    }


    void ChatClient::chatLoop()
    {
        chatMenu();

        while(chatMenuState_)
        {
            chatMenuChoice();
        }
    }

    void ChatClient::chatMenu()
    {
        std::cout << "show command list >>> " << std::endl;
        for (auto &command : commandMap_)
        {
            std::cout << command.first << " : " << command.second << std::endl;
        }
        std::cout << std::endl;
    }

    void ChatClient::chatMenuChoice()
    {
        char buffer[1024] = {0};
        std::cin.getline(buffer, 1024);

        std::string commandbuf(buffer);
        std::string command; // 存储命令
        int idx = commandbuf.find(":");
        if (-1 == idx)
        {
            command = commandbuf;
        }
        else
        {
            command = commandbuf.substr(0, idx);
        }
        auto it = commandHandlerMap_.find(command);
        if (it == commandHandlerMap_.end())
        {
            std::cerr << "invalid input command!" << std::endl;
        }
        else
        {
            chatCommand_ = commandbuf.substr(idx + 1, commandbuf.size() - idx);
            // 调用相应命令的事件处理回调，mainMenu对修改封闭，添加新功能不需要修改该函数
            it->second(); // 调用命令处理方法
            chatCommand_ = "";
        }    
    }


    void ChatClient::oneOnOneChat()
    {
        int idx = chatCommand_.find(":"); // friendid:message
        if (-1 == idx)
        {
            std::cerr << "chat command invalid!" << std::endl;
            return;
        }

        int friendid = atoi(chatCommand_.substr(0, idx).c_str());
        std::string message = chatCommand_.substr(idx + 1, chatCommand_.size() - idx);

        json js;
        js["msgId"] = ONE_ON_ONE_CHAT;
        js["localId"] = currentUser_.getId();
        js["name"] = currentUser_.getName();
        js["peerId"] = friendid;
        js["peerMsg"] = message;
        js["MsgSendTime"] = getCurrentTime();
        std::string buffer = js.dump();

        int len = send(clientFd_, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
        if (-1 == len)
        {
            std::cerr << "send chat msg error -> " << buffer << std::endl;
        }        
    }

    void ChatClient::addFriend()
    {
        int friendId = atoi(chatCommand_.c_str());
        json js;
        js["msgId"] = ADD_FRIEND;
        js["localId"] = currentUser_.getId();
        js["peerId"] = friendId;
        std::string buffer = js.dump();

        int ret = send(clientFd_, buffer.c_str(), strlen(buffer.c_str())+1, 0);
        if(ret == -1)
        {
            std::cerr << "send addfriend msg error -> " << buffer << std::endl;
            return ;
        }
        sem_wait(&rwsem_);
    }

    void ChatClient::createGroup()
    {
        int idx = chatCommand_.find(":");
        if (-1 == idx)
        {
            std::cerr << "creategroup command invalid!" << std::endl;
            return;
        }

        std::string groupName = chatCommand_.substr(0, idx);
        std::string groupDesc = chatCommand_.substr(idx + 1, chatCommand_.size() - idx);

        json js;
        js["msgId"] = CREATE_GROUP;
        js["localId"] = currentUser_.getId();
        js["groupName"] = groupName;
        js["groupDesc"] = groupDesc;
        std::string buffer = js.dump();

        int len = send(clientFd_, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
        if (-1 == len)
        {
            std::cerr << "send creategroup msg error -> " << buffer <<  std::endl;
            return ;
        }
        sem_wait(&rwsem_);

    }

    void ChatClient::addGroup()
    {
        int groupid = atoi(chatCommand_.c_str());
        json js;
        js["msgId"] = ADD_GROUP;
        js["localId"] = currentUser_.getId();
        js["groupId"] = groupid;
        js["groupRole"] = "normal";
        std::string buffer = js.dump();

        int len = send(clientFd_, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
        if (-1 == len)
        {
            std::cerr << "send addgroup msg error -> " << buffer << std::endl;
        }
        sem_wait(&rwsem_);
    }

    void ChatClient::groupChat()
    {
        int idx = chatCommand_.find(":");
        if (-1 == idx)
        {
            std::cerr << "groupchat command invalid!" << std::endl;
            return;
        }

        int groupId = atoi(chatCommand_.substr(0, idx).c_str());
        std::string message = chatCommand_.substr(idx + 1, chatCommand_.size() - idx);

        json js;
        js["msgId"] = GROUP_CHAT;
        js["localId"] = currentUser_.getId();
        js["name"] = currentUser_.getName();
        js["groupId"] = groupId;
        js["GroupMsg"] = message;
        js["MsgSendTime"] = getCurrentTime();
        std::string buffer = js.dump();

        int len = send(clientFd_, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
        if (-1 == len)
        {
            std::cerr << "send groupchat msg error -> " << buffer << std::endl;
        }        
    }

    void ChatClient::loginOut()
    {
        json js;
        js["msgId"] = LOGOUT;
        js["localId"] = currentUser_.getId();
        std::string buffer = js.dump();

        int len = send(clientFd_, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
        if (-1 == len)
        {
            std::cerr << "send loginout msg error -> " << buffer << std::endl;
        }
        else
        {
            chatMenuState_ = false;
        }
    }



    std::string ChatClient::getCurrentTime()
    {
        auto tt = time(0);
        struct tm *ptm = localtime(&tt);
        char date[60] = {0};
        sprintf(date, "%d-%02d-%02d %02d:%02d:%02d",
                (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
                (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
        return std::string(date);
    }


    void ChatClient::readTaskHandler()
    {
        while (1)
        {
            char buffer[1024] = {0};
            int len = recv(clientFd_, buffer, 1024, 0);  // 阻塞了
            if (-1 == len || 0 == len)
            {
                exit(-1);
            }

            // 接收ChatServer转发的数据，反序列化生成json数据对象
            json js = json::parse(buffer);
            int msgType = js["msgId"].get<int>();

            switch (msgType)
            {
                case LOGIN_ACK:
                    loginResponse(js); // 处理登录响应的业务逻辑
                    sem_post(&rwsem_);    // 通知主线程，登录结果处理完成
                break;
                case REGISTR_ACK:
                    registerResponse(js); // 处理注册响应的业务逻辑
                    sem_post(&rwsem_);    // 通知主线程，登录结果处理完成
                break;
                case ADD_FRIEND_ACK:
                    addFriendResponse(js); // 处理添加好友响应的业务逻辑
                    sem_post(&rwsem_);    // 通知主线程，登录结果处理完成
                break;
                case DEL_FRIEND_ACK:
                    delFriendResponse(js); // 处理删除好友的业务逻辑
                    sem_post(&rwsem_);    // 通知主线程，登录结果处理完成
                break;
                case ONE_ON_ONE_CHAT_ACK:
                    oneOnOneChatResponse(js); // 处理一对一聊天响应的业务逻辑
                break;
                case CREATE_GROUP_ACK:
                    creatGroupResponse(js); // 处理创建群聊响应的业务逻辑
                    sem_post(&rwsem_);    // 通知主线程，登录结果处理完成
                break;
                case ADD_GROUP_ACK:
                    addGroupResponse(js); // 处理添加群聊响应的业务逻辑
                    sem_post(&rwsem_);    // 通知主线程，登录结果处理完成
                break;
                case GROUP_CHAT_ACK:
                    groupChatResponse(js); // 处理群聊聊天响应的业务逻辑
                break;
                default:
                break;
            }
        }
    }

    void ChatClient::loginResponse(json &js)
    {
        if (js["errno"].get<int>() < 0 ) // 登录失败
        {
            std::cerr << js["errmsg"] << std::endl;
            loginState_ = false;
        }
        else
        {
            currentUser_.setId(js["localId"].get<int>());
            currentUser_.setName(js["name"]);

            // 记录当前用户的好友列表信息
            if (js.contains("friendsLists"))
            {
                // 初始化
                currentUserFriends_.clear();

                std::vector<std::string> userVec = js["friendsLists"];
                for (std::string &str : userVec)
                {
                    json strjs = json::parse(str);
                    User user;
                    user.setId(strjs["peerId"].get<int>());
                    user.setName(strjs["name"]);
                    user.setState(strjs["state"]);
                    currentUserFriends_.emplace_back(user);
                }
            }

            if(js.contains("GroupUserLists"))
            {
                currentUserGroups_.clear();

                std::vector<std::string> usergroupVec = js["GroupUserLists"];

                for(std::string &str: usergroupVec)
                {
                    json strjs = json::parse(str);
                    Group group;
                    group.setGroupId(strjs["groupId"].get<int>());
                    group.setGroupName(strjs["groupName"]);
                    group.setGroupDesc(strjs["groupDesc"]);

                    std::vector<std::string> groupUserVec = strjs["GroupUser"];
                    for(std::string &userStr: groupUserVec)
                    {
                        json userStrJs = json::parse(userStr);
                        GroupUser groupUser;
                        groupUser.setId(userStrJs["localId"].get<int>());
                        groupUser.setName(userStrJs["name"]);
                        groupUser.setState(userStrJs["state"]);
                        groupUser.setGroupRole(userStrJs["groupRole"]);
                        group.setGroupUser(groupUser);
                    }
                    currentUserGroups_.emplace_back(group);
                }
            }

            if(js.contains("offlineMsg"))
            {
                std::vector<std::string> offlineMsgVec = js["offlineMsg"];
                for(std::string &str: offlineMsgVec)
                {
                    json offlineMsgJs = json::parse(str);
                    if(offlineMsgJs["msgId"] == ONE_ON_ONE_CHAT)
                    {
                        std::cout << offlineMsgJs["MsgSendTime"].get<std::string>() << " [" << offlineMsgJs["peerId"] << "]"
                                << " said: " << offlineMsgJs["peerMsg"].get<std::string>() << std::endl;
                    }
                    else if(offlineMsgJs["msgId"] == GROUP_CHAT)
                    {
                        std::cout << "群消息[" << offlineMsgJs["groupId"] << "]:" << offlineMsgJs["MsgSendTime"].get<std::string>() << " [" << offlineMsgJs["peerId"] << "]"
                            << " said: " << offlineMsgJs["GroupMsg"].get<std::string>() << std::endl;
                    }

                }
            }
            loginState_ = true;
        }
    }

    void ChatClient::registerResponse(json &js)
    {
         
         if(js["errno"].get<int>() < 0)
         {
            std::cout << "register failed maybe name is already exist" <<std::endl;
         }
         else
         {
            std::cout << "register success, you userId is " << js["localId"].get<int>() << std::endl;
         }
    }

    void ChatClient::addFriendResponse(json &js)
    {
        std::cout << js["AddFriendInfo"].get<std::string>() << std::endl;
    }

    void ChatClient::delFriendResponse(json &js)
    {
        std::cout << js["deleteFriendInfo"].get<std::string>() << std::endl;
    }

    void ChatClient::oneOnOneChatResponse(json &js)
    {
        std::cout << js["MsgSendTime"].get<std::string>() << " [" << js["peerId"] << "]"
        << " said: " << js["peerMsg"].get<std::string>() << std::endl;
    }

    void ChatClient::creatGroupResponse(json &js)
    {   
        if(js["errno"] < 0)
        {
            std::cout << js["CreateGroupInfo"].get<std::string>() << std::endl;
        }
        else
        {
            std::cout << js["CreateGroupInfo"].get<std::string>() << "corespondingGroupId" <<js["corespondingGroupId"].get<int>() << std::endl;
        }
    }

    void ChatClient::addGroupResponse(json &js)
    {

        std::cout << js["addGroupInfo"].get<std::string>() << js["corespondingGroupId"].get<int>() << std::endl;
    }

    void ChatClient::groupChatResponse(json &js)
    {
        std::cout << "群消息[" << js["groupId"] << "]:" << js["MsgSendTime"].get<std::string>() << " [" << js["peerId"] << "]" 
        << " said: " << js["GroupMsg"].get<std::string>() << std::endl;
    }




} // namespace chatRoom