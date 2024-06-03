#include "../include/client.hpp"

Client::Client(int fd): _fd(fd), _pwdClear(false), _registred(false) {

}
Client::Client(): _fd(-1), _pwdClear(false), _registred(false){}

Client::~Client(){

}

const std::string&  Client::getNickname()const {return this->_nickname;}
const std::string&  Client::getUsername()const {return this->_username;}
const std::string&  Client::getHostname()const {return this->_hostname;}
const bool&         Client::getRegistred()const {return this->_registred;}
const bool&         Client::getPwdlClear()const {return this->_pwdClear;}
const std::vector<std::string>& Client::getEveryChannel()const {return this->_channels;}

void    Client::setRegistred() {
    this->_registred = true;
}

void    Client::setNickname(std::string nickname) {
    this->_nickname = nickname;
}

void    Client::setUsername(std::string username) {
    this->_username = username;
}

void    Client::setHostname(std::string hostname) {
    this->_hostname = hostname;
}

void    Client::setPwdClear() {
    this->_pwdClear = true;
    if (!this->_nickname.empty() && !this->_username.empty() && this->_pwdClear)
        this->_registred = true;
}

void    Client::addChannel(std::string channelName) {
    this->_channels.push_back(channelName);
}

void    Client::delChannel(std::string channelName) {
    std::vector<std::string>::iterator it = std::find(this->_channels.begin(), this->_channels.end(), channelName);
    if (it != this->_channels.end())
        this->_channels.erase(it);
}

std::string Client::makePrefix() {
    return (":" + this->_nickname + "!" + this->_username + "@" + this->_hostname);
}