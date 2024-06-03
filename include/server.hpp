#ifndef SERVER_HPP
#define SERVER_HPP

#include <sstream>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <stdexcept>
#include <cstdlib>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <vector>
#include <poll.h>
#include <map>
#include "client.hpp"
#include "channel.hpp"
#include "rply.hpp"
#include <csignal>

class Server {
    private:
        std::string                 _pwd;
        int                         _port;
        int                         _sock;
        struct sockaddr_in          _addr;
        std::vector<struct pollfd>  _fds;
        std::map<int, Client>       _clients;
        std::map<std::string, Channel>      _channels;

        Server();
        static void    _stop(int sig);
        void    _setup();
        void    _handleNewClient();
        void    _handleClient(int fd);
        void    _removeClient(int fd);
        void    _processCommand(int fd, std::string cmdBuffer);
        bool    _isNickAvailable(std::string nick);
        bool    _doesChannelExist(std::string channelName);
        int     _getClientFd(std::string nickName);
        void    _clientQuit(int fd, std::string message);

        void    _nick(int fd, std::string buffer);
        void    _user(int fd, std::string buffer);
        void    _join(int fd, std::string buffer);
        void    _pass(int fd, std::string buffer);
        void    _privmsg(int fd, std::string buffer);
        void    _quit(int fd, std::string buffer);
        void    _ping(int fd, std::string buffer);
        void    _mode(int fd, std::string buffer);
        void    _topic(int fd, std::string buffer);
        void    _invite(int fd, std::string buffer);
        void    _kick(int fd, std::string buffer);
        void    _part(int fd, std::string buffer);

        void    _sendMessage(int fd, std::string message);
        void    _sendError(int fd, std::string code, std::string message);
        void    _sendRply(int fd, std::string code, std::string message);
        void    _sendToChannel(int src, std::string channel, std::string message);
        std::string getNameList(std::string channnel);
    public:
        Server(int port, std::string pwd);
        ~Server();
        void    run();
};

#endif