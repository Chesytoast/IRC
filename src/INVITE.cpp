#include "../include/server.hpp"

void    Server::_invite(int fd, std::string buffer) {
    std::stringstream ss(buffer);
    std::string command;
    ss >> command;

    std::string channelName, nick;
    ss >> nick;
    if (nick.empty()) {
        _sendError(fd, ERR_NEEDMOREPARAMS, command + " :Not enough parameters");
        return;
    }
    if (_isNickAvailable(nick)) {
        _sendError(fd, ERR_NOSUCHNICK, nick + " :No such nick");
        return;
    }
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
    //protect if user want to use command if he is no a member
    if (!channel.isMember(fd)) {
        _sendError(fd, ERR_NOTONCHANNEL, this->_clients[fd].getNickname() + " " + channelName + " :You're not on that channel");
        return;
    }
    if (channel.getInviteOnly() && !channel.isOperator(fd)) {
        _sendError(fd, ERR_CHANOPRIVSNEEDED, channelName + " :You're not channel operator");
        return;
    }
    int receiver = _getClientFd(nick);
    if (channel.isMember(receiver)) {
        _sendError(fd, ERR_USERONCHANNEL, nick + " " + channelName + " :is already on channel");
        return;
    }
    //send invite
    _sendRply(fd, RPL_INVITING, this->_clients[fd].getNickname() + " " + channelName + " " + nick);
    _sendMessage(receiver, this->_clients[fd].makePrefix() + " INVITE " + nick + " " + channelName);
    channel.invite(fd);
}