#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <stdexcept>
#include <algorithm>

class Channel {
    private:
        std::string         _name;
        std::vector<int>    _clients;

        std::string         _topic;
        std::string         _pwd;
        bool                _inviteOnly;
        bool                _topicOpOnly;
        std::vector<int>    _operators;
        std::vector<int>    _invitedClients;
        size_t              _userLimit;

    public:
        Channel(std::string name);
        Channel();
        ~Channel();

        const std::string&  getName()const;
        const std::vector<int>& getClients()const;
        const std::string&  getTopic()const;
        const bool& getInviteOnly()const;
        const bool& getTopicOponly()const;

        void    addClient(int fd);
        void    delClient(int fd);
        size_t  getUsers();

        void    addOperator(int fd);
        void    delOperator(int fd);

        void    inviteClient(int fd);
        void    setInviteOnly(bool b);
        void    setTopicOpOnly(bool b);
        void    setTopic(std::string topic);
        void    setUserlimit(size_t limit);
        void    setPwd(std::string pwd);
        void    clearPwd();
        void    invite(int fd);

        bool    isLimitReached();
        bool    isMember(int fd);
        bool    isOperator(int fd);
        bool    isPwdCorrect(std::string pwd);
        bool    isInvited(int fd);
};

#endif