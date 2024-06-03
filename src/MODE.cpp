#include "../include/server.hpp"

void    Server::_mode(int fd, std::string buffer) {
    std::stringstream ss(buffer);
    std::string command;
    ss >> command;

    std::string channelName, mode, arg;
    ss >> channelName;
    if (channelName.empty()) {
        _sendError(fd, ERR_NEEDMOREPARAMS, command + " :Not enough parameters");
        return;
    }
    if (channelName == this->_clients[fd].getNickname()) {
        return;
    }
    if (!this->_clients[fd].getRegistred()){_sendError(fd, ERR_NOTREGISTERED, " :You have not registered");return;}
    if (!_doesChannelExist(channelName)) {
        _sendError(fd, ERR_NOSUCHHANNEL, channelName + " :No such channel");
        return;
    }
    Channel& channel = this->_channels[channelName];
    //verifier si op
    //
    ss >> mode;
    //info de mode
    if (mode.empty()) {
        if (channel.isOperator(fd)){
            _sendRply(fd, RPL_CHANNELMODEIS, this->_clients[fd].getNickname() + " " + channelName + " +o");
        }
        else {
            _sendRply(fd, RPL_CHANNELMODEIS, this->_clients[fd].getNickname() + " " + channelName + " -o");
        }
        return;
    }
    if (!channel.isOperator(fd)) {
        _sendError(fd, ERR_CHANOPRIVSNEEDED, channelName + " :You're not channel operator");
        return;
    }
    if (mode[0] != '+' && mode[0] != '-') {
        _sendError(fd, ERR_UNKNOWNMODE,  mode.substr(0, 1) + " :is unknown mode char to me for <channel>");
        return;
    }
    for (size_t i = 1; i < mode.size(); i++){
        switch (mode[i]) {
            case 'i':
                if (mode[0] == '+'){
                    channel.setInviteOnly(true);
                }
                else {
                    channel.setInviteOnly(false);
                }
                break;
            case 't':
                if (mode[0] == '+'){
                    channel.setTopicOpOnly(true);
                }
                else {
                    channel.setTopicOpOnly(false);  
                }
                break;
            case 'k':
                if (mode[0] == '+'){
                    ss >> arg;
                    if (arg.empty()) {
                        _sendError(fd, ERR_NEEDMOREPARAMS, command + " :Not enough parameters");
                            return;
                    }
                    channel.setPwd(arg);
                }
                else {
                    channel.clearPwd();
                }
                break;
            case 'o':
                ss >> arg;
                if (arg.empty()) {
                    _sendError(fd, ERR_NEEDMOREPARAMS, command + " :Not enough parameters");
                    return;
                }
                //arg to fd
                int newOp;
                if ((newOp = _getClientFd(arg)) < 0){
                    _sendError(fd, ERR_USERNOTINCHANNEL, arg + " " + channelName +" :They aren't on that channel");
                    return;
                }
                if (!channel.isMember(newOp)) {
                    _sendError(fd, ERR_USERNOTINCHANNEL, arg + " " + channelName +" :They aren't on that channel");
                    return;
                }
                if (mode[0] == '+'){
                    channel.addOperator(newOp);
                }
                else {
                    channel.delOperator(newOp);    
                }
                break;
            case 'l':
                if (mode[0] == '+'){
                    size_t limit = 0;
                    ss >> limit;
                    if (!limit) {
                        _sendError(fd, ERR_NEEDMOREPARAMS, command + " :Not enough parameters");
                        return;
                    }
                    channel.setUserlimit(limit);
                }
                else {
                    channel.setUserlimit(0); 
                }
                break;
            default:
                _sendError(fd, ERR_UNKNOWNMODE,  mode.substr(i, 1) + " :is unknown mode char to me for <channel>");
                return;
            }
    }
}
