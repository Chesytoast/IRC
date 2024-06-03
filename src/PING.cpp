#include "../include/server.hpp"

void    Server::_ping(int fd, std::string buffer) {
    std::stringstream ss(buffer);
    std::string command;
    ss >> command;

    std::string message;
    ss >> message;
    std::string wholeMessage = ":ircserv PONG " + message;
    _sendMessage(fd, wholeMessage);
}