#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <string>
#include <semaphore.h>
#include <atomic>
#include <unordered_map>
#include <functional>

#include "user.h"
#include "group.h"
#include "json.hpp"
using json = nlohmann::json;


namespace chatRoom
{
    /**
     * @brief 客户端封装类
     * @details 主线程负责和用户交互，子线程负责接受回应
     */
    class ChatClient
    {
    public:
        using CommandHandler = std::function<void(void)>;
        ChatClient(std::string ip, int port);
        ~ChatClient();

        /**
         * @brief 和服务器建立连接，连接建立后呈现主界面，并子线程等待接受回应消息
         */
        void start();

        /**
         * @brief 循环执行对应的请求
         */
        void loop();
    
    private:
        /**
         * @brief 和服务器建立连接
         */        
        void chatConnect();

        /**
         * @brief 呈现主界面
         */         
        void mainMenu();

        /**
         * @brief 主界面对应的请求事件的分发处理函数
         */           
        void mainMenuChoice();

        /**
         * @brief 主界面对应的登录的处理函数
         */   
        void mainMenulogin();

        /**
         * @brief 主界面对应的注册的处理函数
         */   
        void mainMenuRegister();

        /**
         * @brief 主界面对应的退出的处理函数
         */   
        void mainMenuQuit();

        /**
         * @brief 主界面对应的异常请求的处理函数
         */   
        void mainMenuError();


        /**
         * @brief 登入成功后进行聊天循环
         */   
        void chatLoop();

        /**
         * @brief 登入成功后的界面
         */ 
        void chatMenu();

        /**
         * @brief 聊天界面对应的请求事件的分发处理函数
         */ 
        void chatMenuChoice();

        /**
         * @brief 聊天界面的一对一聊天请求的处理函数
         */         
        void oneOnOneChat();

        /**
         * @brief 聊天界面的添加好友请求的处理函数
         */   
        void addFriend();

        /**
         * @brief 聊天界面的创建群聊聊天请求的处理函数
         */   
        void createGroup();

        /**
         * @brief 聊天界面的加入群聊聊天请求的处理函数
         */   
        void addGroup();

        /**
         * @brief 聊天界面的群聊请求的处理函数
         */   
        void groupChat();

        /**
         * @brief 退出聊天界面的请求的处理函数
         */   
        void loginOut();

        /**
         * @brief 获取当前时间
         */   
        std::string getCurrentTime();

        /**
         * @brief 子线程的执行函数
         * @details 子线程负责处理回应消息
         */  
        void readTaskHandler();

        /**
         * @brief 子线程处理登录的响应函数
         */  
        void loginResponse(json &js);

        /**
         * @brief 子线程处理注册的响应函数
         */  
        void registerResponse(json &js);

        /**
         * @brief 子线程处理添加朋友的响应函数
         */  
        void addFriendResponse(json &js);

        /**
         * @brief 子线程处理删除好友的响应函数
         */  
        void delFriendResponse(json &js);

        /**
         * @brief 子线程处理一对一聊天的响应函数
         */  
        void oneOnOneChatResponse(json &js);

        /**
         * @brief 子线程处理创建群聊的响应函数
         */  
        void creatGroupResponse(json &js);

        /**
         * @brief 子线程处理加入群聊的响应函数
         */  
        void addGroupResponse(json &js);

        /**
         * @brief 子线程处理群聊的响应函数
         */  
        void groupChatResponse(json &js);

        sockaddr_in serverInetAddr_;
        int clientFd_;
        sem_t rwsem_;
        bool mainMenuLoopState_;
        std::atomic_bool loginState_;
        bool chatMenuState_;
        std::string chatCommand_;
        /// 当前登录成功的用户
        User currentUser_;
        /// 当前登录成功的用户的好友列表
        std::vector<User> currentUserFriends_;
        /// 当前登录成功的用户的群列表
        std::vector<Group> currentUserGroups_;
        /// 聊天界面
        std::unordered_map<std::string, std::string> commandMap_;
        /// 聊天界面的请求对应的处理函数
        std::unordered_map<std::string, CommandHandler> commandHandlerMap_;
    };
} // namespace chatRoom
