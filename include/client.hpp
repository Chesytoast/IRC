#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>
#include <algorithm>

class Client {
    private:
        int         _fd;
        bool        _pwdClear;
        bool        _registred;
        std::string _nickname;
        std::string _username;
        std::string _hostname;
        std::string _realname;
        std::vector<std::string> _channels;

    public:
        std::string buffer;
        Client(int fd);
        Client();
        ~Client();

        const std::string&  getNickname()const;
        const std::string&  getUsername()const;
        const std::string&  getHostname()const;
        const bool&         getRegistred()const;
        const bool&         getPwdlClear()const;
        void    setRegistred();
        void    setUsername(std::string username);
        void    setHostname(std::string hostname);
        void    setNickname(std::string nickname);
        void    setPwdClear();
        const std::vector<std::string>&    getEveryChannel() const;
        std::string makePrefix();

        void    addChannel(std::string channelName);
        void    delChannel(std::string channelName);

};

#endif