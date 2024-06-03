#include "../include/server.hpp"

void    Server::_part(int fd, std::string buffer) {
    std::stringstream ss(buffer);
    std::string command;
    ss >> command;

    std::string channelName, arg;
    ss >> channelName;
    if (channelName.empty()) {
        _sendError(fd, ERR_NEEDMOREPARAMS, command + " :Not enough parameters");
        return;
    }
    if (!_doesChannelExist(channelName)) {
        _sendError(fd, ERR_NOSUCHHANNEL, channelName + " :No such channel");
        return;
    } 
    Channel& channel = this->_channels[channelName];
    
    if (!channel.isMember(fd)) {
        _sendError(fd, ERR_NOTONCHANNEL, this->_clients[fd].getNickname() + " " + channelName + " :You're not on that channel");
        return;
    }
    std::string message;
    std::getline(ss, message); 
    if (!message.empty() && message[0] == ' ')
        message.erase(0, 1);
    _sendMessage(fd, this->_clients[fd].makePrefix() + " PART " + channelName);
    _sendToChannel(fd, channelName, this->_clients[fd].makePrefix() + " PART " + channelName + message);
    channel.delClient(fd);
}