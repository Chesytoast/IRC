#include "../include/server.hpp"

void    Server::_nick(int fd, std::string buffer) {
    std::stringstream ss(buffer);
    std::string command;

    ss >> command;
    std::string nickname;
    ss >> nickname;
    //eventuellement d'autres check
    if (nickname.empty()) {
        //error
        _sendError(fd, ERR_NONICKNAMEGIVEN, " :No nickname given");
        return;
    }
    if (!_isNickAvailable(nickname)) {
        _sendError(fd, ERR_NICKNAMEINUSE, this->_clients[fd].getNickname() + " :Nickname is already in use");
        return;
    }
    std::string oldnick = this->_clients[fd].makePrefix();
    this->_clients[fd].setNickname(nickname);
    if (this->_clients[fd].getRegistred()) {
        _sendMessage(fd, oldnick + " NICK " + nickname );
    }
    else if (!this->_clients[fd].getUsername().empty() && this->_clients[fd].getPwdlClear()) {
        //send welcome
        this->_clients[fd].setRegistred();
        _sendRply(fd, RPLY_WELCOME, this->_clients[fd].getNickname() + " :Welcome to the Internet Relay Network");
    }
    return;
}