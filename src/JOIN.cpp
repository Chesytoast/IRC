#include "../include/server.hpp"

void    Server::_join(int fd, std::string buffer) {
    std::stringstream ss(buffer);
    std::string command;
    ss >> command;

    std::string channelName;
    ss >> channelName;
    if (channelName.empty()) {
        _sendError(fd, ERR_NEEDMOREPARAMS, command + " :Not enough parameters");
        return;
    }
    std::string pwd;
    ss >> pwd;
    if (!_doesChannelExist(channelName)) {
        this->_channels.insert(std::pair<std::string, Channel>(channelName, Channel(channelName)));
        this->_channels[channelName].addOperator(fd);
    }
    //Use channel for ease of use
    Channel& channel = this->_channels[channelName];
    //verif a faire avec les droits
    if (channel.getInviteOnly() && !channel.isInvited(fd)) {
        _sendError(fd, ERR_INVITEONLYCHAN, ":Cannot join channel (+l)");
        return;
    }
    if (!channel.isPwdCorrect(pwd)) {
        _sendError(fd, ERR_BADCHANNELKEY, ":Cannot join channel (+k)");
        return;
    }
    if (channel.isLimitReached()) {
        _sendError(fd, ERR_CHANNELISFULL, ":Cannot join channel (+l)");
        return;
    }
    this->_clients[fd].addChannel(channelName);
    channel.addClient(fd);
    //msg join
    std::string joinMessage = this->_clients[fd].makePrefix() + " JOIN " + channelName;
    _sendMessage(fd, joinMessage);
    _sendToChannel(fd, channelName, joinMessage);
    //rplytopic
    if (!channel.getTopic().empty()) {
        _sendRply(fd, RPLY_TOPIC, channelName + " :No topic is set");
    }
    else {
        _sendRply(fd, RPLY_NOTOPIC, channelName + " :" + channel.getTopic());
    }
    //rplyname
    _sendRply(fd, RPL_NAMREPLY, this->_clients[fd].getNickname() + " = " + channelName + getNameList(channelName));
    //rplyendofname
    _sendRply(fd, RPL_ENDOFNAMES, this->_clients[fd].getNickname() +  " " + channelName + " :End of NAMES list");
}

std::string    Server::getNameList(std::string channelName) {
    Channel& channel = this->_channels[channelName];

    std::string nameList(" : ");
    std::vector<int>    client = channel.getClients();
    for (size_t i = 0; i < client.size(); i++) {
        int fd = client[i];
        if (channel.isOperator(fd)) {
            nameList += "@";
        }
        nameList += this->_clients[fd].getNickname() + " ";
        std::cout << nameList << std::endl;
    }
    return nameList;
}