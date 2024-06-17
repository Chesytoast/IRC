#include "../include/server.hpp"

void    Server::_quit(int fd, std::string buffer) {
    std::stringstream ss(buffer);
    std::string command;
    ss >> command;

    std::string message;
    std::getline(ss, message); 
    if (!message.empty() && message[0] == ' ')
        message.erase(0, 1);
    _clientQuit(fd, message);
}

void    Server::_clientQuit(int fd, std::string message) {
    std::vector<std::string>    channels = this->_clients[fd].getEveryChannel();
    std::vector<std::string>::iterator it;
    //make the quit message format
    Client& client = this->_clients[fd];
    std::string allMsg = client.makePrefix() + " QUIT " + message;
    for (it = channels.begin(); it != channels.end(); it++) {
        //send quit message to all channel the client is a part of
        _sendToChannel(fd, *it, allMsg);
        this->_channels[*it].delClient(fd);
        if (this->_channels[*it].getUsers() == 0){
            this->_channels.erase(*it);
        }
    }
    //send to himself
    _sendMessage(fd, message);
    close(fd);
    _removeClient(fd);
}