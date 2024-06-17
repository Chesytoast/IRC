#include "../include/server.hpp"

void    Server::_kick(int fd, std::string buffer) {
    std::stringstream ss(buffer);
    std::string command;
    ss >> command;

    std::string channelName, nick;
    ss >> channelName;
    if (channelName.empty()) {
        _sendError(fd, ERR_NEEDMOREPARAMS, command + " :Not enough parameters");
        return;
    }
   if (!_doesChannelExist(channelName)) {
        _sendError(fd, ERR_NOSUCHHANNEL, channelName + " :No such channel");
        return;
    }
    ss >> nick;
    if (nick.empty()) {
        _sendError(fd, ERR_NEEDMOREPARAMS, command + " :Not enough parameters");
        return;
    }
    if (_isNickAvailable(nick)) {
        _sendError(fd, ERR_NOSUCHNICK, nick + " :No such nick");
        return;
    }
    Channel& channel = this->_channels[channelName];
    //protect if user want to use command if he is no a member
    if (!channel.isMember(fd)) {
        _sendError(fd, ERR_NOTONCHANNEL, this->_clients[fd].getNickname() + " " + channelName + " :You're not on that channel");
        return;
    }
     if (!channel.isOperator(fd)) {
        _sendError(fd, ERR_CHANOPRIVSNEEDED, channelName + " :You're not channel operator");
        return;
    }
    int dst = _getClientFd(nick);
    if (!channel.isMember(fd)) {
        _sendError(fd, ERR_USERNOTINCHANNEL, nick + " " + channelName +" :They aren't on that channel");
        return;
    }
    //send msg
    std::string comment, msg;
    std::getline(ss, comment);
    msg = this->_clients[fd].makePrefix() + " KICK " + channelName + " " + nick + " " + comment;
    _sendMessage(fd, msg);
    _sendToChannel(fd, channelName, msg);
    channel.delClient(dst);
}