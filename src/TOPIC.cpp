#include "../include/server.hpp"

void    Server::_topic(int fd, std::string buffer) {
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
    if (std::getline(ss, arg)) {
        if (channel.getTopicOponly()) {
            if (channel.isOperator(fd)) {
                //modif
                channel.setTopic(arg);
                std::string toSend = this->_clients[fd].makePrefix() + " TOPIC " + channelName + " :" + arg;
                _sendMessage(fd, toSend);
                // _sendRply(fd, RPL_TOPIC, this->_clients[fd].getNickname() + " " + channelName + " :" + arg );
                _sendToChannel(fd, channelName, toSend);
            }
            else {
                _sendError(fd, ERR_CHANOPRIVSNEEDED, channelName + " :You're not channel operator");
                return;
            }

        }
        else {
            //modif
            channel.setTopic(arg);
            std::string toSend = this->_clients[fd].makePrefix() + " TOPIC " + channelName + arg;
            // if (toSend[0] != ':'){
            //     toSend = ":" + toSend;
            // }
            // std::cout << toSend << "|" <<std::endl;
            _sendMessage(fd, toSend);
            // _sendRply(fd, RPL_TOPIC, this->_clients[fd].getNickname() + " " + channelName + " :" + arg );
            _sendToChannel(fd, channelName, toSend);
        }
    }
    else {
        if (channel.getTopic().empty()){
            //notopic
            _sendRply(fd, RPLY_NOTOPIC, this->_clients[fd].getNickname() + " " + channelName + " :No topic is set" );
        }
        else {
            //topic
            _sendRply(fd, RPLY_TOPIC, this->_clients[fd].getNickname() + " " + channelName+ " :" + channel.getTopic());
        }
    }
}