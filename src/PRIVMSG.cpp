#include "../include/server.hpp"

void    Server::_privmsg(int fd, std::string buffer) {
    std::stringstream ss(buffer);
    std::string command;
    ss >> command;

    std::string dest, message;
    ss >> dest;
    std::getline(ss, message);
    //soit le destinataire est une personne ou un salon
    //1 personne: envoie du message au fd avec send
    if (dest.c_str()[0] != '#') {
        //check si le dest existe et recup son fd;
        int fdToSend;
        if ((fdToSend = _getClientFd(dest)) < 0) {
            _sendError(fd, ERR_NOSUCHNICK, dest + " :No such nick");
            return;
        }
        _sendMessage(fdToSend, this->_clients[fd].makePrefix() + " PRIVMSG " + dest + " :" + message);
    }
    //2 envoi du message sur le channel -> boucler sur tout les clients du channel et envoyer le msg
    else {
        //check si le dest channel existe;
        if (!_doesChannelExist(dest)) {
            _sendError(fd, ERR_NOSUCHHANNEL, dest + " :No such channel");
            return;
        }
        if (!this->_channels[dest].isMember(fd)) {
            //std::cout << "here" << std::endl;
            _sendError(fd, ERR_NOTONCHANNEL, this->_clients[fd].getNickname() + " " + dest + " :You're not on that channel");
            return;
        }
        _sendToChannel(fd, dest, this->_clients[fd].makePrefix() + " PRIVMSG " + dest + " :" + message);
    }
}