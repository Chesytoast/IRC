#include "../include/server.hpp"

static bool shouldRun = true;

static void stop(int) {
    shouldRun = false;
}

Server::~Server() {
    if (this->_sock < 0)
        return;
    close(this->_sock);
    //erase all client;
    for (size_t i = 0; i < this->_fds.size(); ++i) {
        close(this->_fds[i].fd);
    }
    this->_fds.clear();
}


Server::Server(int port, std::string pwd): _pwd(pwd), _port(port), _sock(-1) {
    this->_setup();
}

void    Server::_setup() {
    this->_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_sock < 0) {
        throw std::runtime_error("Server socket failure.");
    }

    int opt = 1;
    if (setsockopt(this->_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(this->_sock);
        throw std::runtime_error("Server setsockopt failure.");
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(this->_port);

    if (bind(this->_sock, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        close(this->_sock);
        throw std::runtime_error("Server bind failure.");
    }

    if (listen(this->_sock, 10) < 0) {
        close(this->_sock);
        throw std::runtime_error("Server listen failure.");
    }

    fcntl(this->_sock, F_SETFL, O_NONBLOCK);

    struct pollfd serverPoll;
    serverPoll.fd = this->_sock;
    serverPoll.events = POLLIN;
    this->_fds.push_back(serverPoll);
}


void    Server::run(){
    signal(SIGINT, stop);
    while (shouldRun) {
        if (poll(&this->_fds[0], this->_fds.size(), -1) < 0) {
            throw std::runtime_error("Poll failure or interrupted manually");
        }
        for (size_t i = 0; i < this->_fds.size(); i++) {
            if (this->_fds[i].revents & POLLIN) {
                if (this->_fds[i].fd == this->_sock) {
                    this->_handleNewClient();
                }
                else {
                    this->_handleClient(this->_fds[i].fd);
                }
            }
        }
    }
}

void    Server::_handleNewClient() {
    sockaddr_in clientAddress;
    memset(&clientAddress, 0, sizeof(clientAddress));
    socklen_t addrSize = sizeof(clientAddress);

    int clientFd = accept(this->_sock, (struct sockaddr*)&clientAddress, &addrSize);
    if (clientFd < 0) {
        std::cerr << "Error while accepting new client" << std::endl;
        return;
    }
    fcntl(clientFd, F_SETFL, O_NONBLOCK);

    struct pollfd clientPoll;
    clientPoll.fd = clientFd;
    clientPoll.events = POLLIN;
    this->_fds.push_back(clientPoll);

    this->_clients.insert(std::pair<int, Client>(clientFd, Client()));

    //std::cout << "Client accpeted with fd: " << clientFd << std::endl; 
}


void    Server::_handleClient(int fd) {
    char buffer[1024];
    ssize_t bytesRead;

    memset(buffer, 0, 1024);
    bytesRead = recv(fd, buffer, sizeof(buffer), 0);
    if (bytesRead < 0) {
        std::cerr << "Error receiving data from client " << fd << std::endl;
        return;
    }
    else if (bytesRead == 0) {
        // std::cout << "Client " << fd << " disconnected." << std::endl;
        _clientQuit(fd, "");
        return;
    }
    //test pour voir ce que recoit le server
    buffer[bytesRead] = 0;
    // write(1, "[", 1);
    // write(1, buffer, bytesRead);
    // write(1, "]\n", 2);

    //parsing et exec des cmd
    this->_clients[fd].buffer.append(buffer,bytesRead);
    std::string cmdBuffer;
    size_t  pos = 0;

    while ((pos = this->_clients[fd].buffer.find("\n")) != std::string::npos) {
        cmdBuffer = this->_clients[fd].buffer.substr(0, pos - 1);
        _processCommand(fd, cmdBuffer);
        this->_clients[fd].buffer.erase(0, pos + 1);
    }
}

void    Server::_processCommand(int fd, std::string cmdBuffer) {
    // std::cout << "Client [" << this->_clients[fd].getNickname() << "] cmd: " << cmdBuffer << std::endl;
    
    std::stringstream ss(cmdBuffer);
    std::string command;

    ss >> command;
    //protection de toutes les commandes
    // if (_unknownCommand(command)) {
    //     _sendError();
    //     return;
    // }
    
    if (command == "CAP") {
        return;
    }
    if (command == "WHO") {
        return;
    }
    else if (command == "PING") {
        _ping(fd, cmdBuffer);
        return;
    }
    else if (command == "NICK") {
        _nick(fd, cmdBuffer);
        return;
    }
    else if (command == "USER") {
        _user(fd, cmdBuffer);
        return;
    }
    else if (command == "PASS") {
        _pass(fd,cmdBuffer);
        return;
    }
    else if (command == "QUIT") {
        _quit(fd, cmdBuffer);
        return;
    }
   //here
    else if (command == "JOIN") {
        if (!this->_clients[fd].getRegistred()){_sendError(fd, ERR_NOTREGISTERED, " :You have not registered");return;}
        _join(fd, cmdBuffer);
        return;
    }
    else if (command == "PRIVMSG") {
        if (!this->_clients[fd].getRegistred()){_sendError(fd, ERR_NOTREGISTERED, " :You have not registered");return;}
        _privmsg(fd, cmdBuffer);
        return;
    }
    else if (command == "MODE") {
        _mode(fd, cmdBuffer);
        return;
    }
    //part
    //topic
    //kick
    //invite
    else if (command == "PART") {
        if (!this->_clients[fd].getRegistred()){_sendError(fd, ERR_NOTREGISTERED, " :You have not registered");return;}
        _part(fd, cmdBuffer);
        return;
    }
    else if (command == "TOPIC") {
        if (!this->_clients[fd].getRegistred()){_sendError(fd, ERR_NOTREGISTERED, " :You have not registered");return;}
        _topic(fd, cmdBuffer);
        return;
    }
    else if (command == "INVITE") {
        if (!this->_clients[fd].getRegistred()){_sendError(fd, ERR_NOTREGISTERED, " :You have not registered");return;}
        _invite(fd, cmdBuffer);
        return;
    }
    else if (command == "KICK") {
        if (!this->_clients[fd].getRegistred()){_sendError(fd, ERR_NOTREGISTERED, " :You have not registered");return;}
        _kick(fd, cmdBuffer);
        return;
    }
    else {
        //unknown command
        _sendError(fd, ERR_UNKNOWNCOMMAND, command + " :Unknown command");
        return;
    }
}


void    Server::_removeClient(int fd) {
    for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end(); ++it) {
        if (it->fd == fd) {
            _fds.erase(it);
            break;
        }
    }
    this->_clients.erase(fd);
}


bool    Server::_isNickAvailable(std::string nick) {
    std::map<int, Client>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); it++){
        if (it->second.getNickname() == nick) {
            return false;
        }
    }
    return true;
}

bool    Server::_doesChannelExist(std::string channelName) {
 std::map<std::string, Channel>::iterator it;
    for (it = _channels.begin(); it != _channels.end(); it++){
        if (it->second.getName() == channelName) {
            return true;
        }
    }
    return false;
}

int Server::_getClientFd(std::string nickName) {
    std::map<int, Client>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); it++){
        if (it->second.getNickname() == nickName) {
            return it->first;
        }
    }
    return -1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//all send

void    Server::_sendMessage(int fd, std::string message) {
    std::string finalMessage = message + "\r\n";
    if (send(fd, finalMessage.c_str(), finalMessage.size(), 0) < 0) {
        throw std::runtime_error("send error");
    }
}

void    Server::_sendError(int fd, std::string code, std::string message) {
    std::string nextMessage = ":ircserv " + code + " " + message;
    _sendMessage(fd, nextMessage);
}

void    Server::_sendRply(int fd, std::string code, std::string message) {
    std::string nextMessage = ":ircserv " + code + " " + message;
    _sendMessage(fd, nextMessage);
}

void    Server::_sendToChannel(int src, std::string channel, std::string message) {
        std::vector<int>    receivers = this->_channels.find(channel)->second.getClients();
        std::vector<int>::iterator it;
        for (it = receivers.begin(); it != receivers.end(); it++) {
            if (*it != src) {
                _sendMessage(*it, message);
            }
        }
}
