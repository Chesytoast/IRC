#include "../include/server.hpp"

void    Server::_pass(int fd, std::string buffer) {
    std::stringstream ss(buffer);
    std::string command;
    ss >> command;

    std::string pass;
        ss >> pass;
    if (pass == this->_pwd) {
        this->_clients[fd].setPwdClear();
        if (this->_clients[fd].getRegistred()) {
            //send welcome
            _sendRply(fd, RPLY_WELCOME, this->_clients[fd].getNickname() + " :Welcome to the Internet Relay Network");
        }
    }
    else {
       _sendError(fd, ERR_PASSWDMISMATCH, " :Password incorrect");
    }
}