#include "../include/server.hpp"

void    Server::_user(int fd, std::string buffer) {
    if (!this->_clients[fd].getUsername().empty()) {
        _sendError(fd, ERR_ALREADYREGISTRED, " :Unauthorized command (already registered)");
        return;
    }
    std::stringstream ss(buffer);
    std::string command;
    ss >> command;

    std::string username, hostname;
    std::string tmp;
    ss >> username;
    if (username.empty()) {
        _sendError(fd, ERR_NEEDMOREPARAMS, " :Not enough parameter");
        return;
    }
    ss >> hostname;
    if (hostname.empty()) {
        _sendError(fd, ERR_NEEDMOREPARAMS, " :Not enough parameter");
        return;
    }
    ss >> tmp;
    if (tmp.empty()) {
        _sendError(fd, ERR_NEEDMOREPARAMS, " :Not enough parameter");
        return;
    }
    tmp.clear();
    ss >> tmp;
    if (tmp.empty()) {
        _sendError(fd, ERR_NEEDMOREPARAMS, " :Not enough parameter");
        return;
    }
    this->_clients[fd].setUsername(username);
    this->_clients[fd].setHostname(hostname);
    if (!this->_clients[fd].getRegistred() && !this->_clients[fd].getNickname().empty() && this->_clients[fd].getPwdlClear()) {
        //send welcome
        this->_clients[fd].setRegistred();
        _sendRply(fd, RPLY_WELCOME, this->_clients[fd].getNickname() + " :Welcome to the Internet Relay Network");
    }
}