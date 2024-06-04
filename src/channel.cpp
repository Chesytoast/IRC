#include "../include/channel.hpp"

Channel::Channel(std::string name): _name(name), _topic(), _pwd(), _inviteOnly(false), _topicOpOnly(false), _userLimit(0){
}

Channel::Channel(): _name(), _topic(), _pwd(), _inviteOnly(false), _topicOpOnly(false), _userLimit(0){}

Channel::~Channel(){}

const std::string&  Channel::getName()const {return this->_name;}
const std::vector<int>& Channel::getClients()const {return this->_clients;}
const bool& Channel::getInviteOnly()const {return this->_inviteOnly;}
const bool& Channel::getTopicOponly()const {return this->_topicOpOnly;}
const std::string& Channel::getTopic()const {return this->_topic;}

bool    Channel::isInvited(int fd) {
    return std::find(this->_invitedClients.begin(), this->_invitedClients.end(), fd) != this->_invitedClients.end();
}

bool    Channel::isMember(int fd) {
    return std::find(this->_clients.begin(), this->_clients.end(), fd) != this->_clients.end();

}

bool    Channel::isOperator(int fd) {
    return std::find(this->_operators.begin(), this->_operators.end(), fd) != this->_operators.end();

}


void    Channel::addClient(int fd) {
    this->_clients.push_back(fd);
    if (isInvited(fd)) {
        std::vector<int>::iterator it = std::find(this->_invitedClients.begin(), this->_invitedClients.end(), fd);
        if (it != this->_invitedClients.end())
            this->_invitedClients.erase(it);
    }
}

void    Channel::delClient(int fd) {
    std::vector<int>::iterator it = std::find(this->_clients.begin(), this->_clients.end(), fd);
    if (it != this->_clients.end())
        this->_clients.erase(it);
    delOperator(fd);

    std::vector<int>::iterator iti = std::find(this->_invitedClients.begin(), this->_invitedClients.end(), fd);
    if (iti != this->_invitedClients.end())
        this->_invitedClients.erase(iti);
}

void    Channel::addOperator(int fd) {
    _operators.push_back(fd);
}

void    Channel::delOperator(int fd) {
    std::vector<int>::iterator it = std::find(this->_operators.begin(), this->_operators.end(), fd);
    if (it != this->_operators.end())
        this->_operators.erase(it);
}

void    Channel::inviteClient(int fd) {
    this->_invitedClients.push_back(fd);
}

void    Channel::setInviteOnly(bool b) {
    this->_inviteOnly = b;
}

void    Channel::setTopicOpOnly(bool b) {
    this->_topicOpOnly = b;
}

void    Channel::setTopic(std::string top) {
    this->_topic = top;
}

void    Channel::setUserlimit(size_t limit) {
    this->_userLimit = limit;
}

bool    Channel::isLimitReached() {
    if (!this->_userLimit)
        return false;
    else
        return this->_userLimit <= this->_clients.size();
}

bool    Channel::isPwdCorrect(std::string pwd) {
    return pwd == this->_pwd;
}

void    Channel::setPwd(std::string pwd) {
    this->_pwd = pwd;
}

void    Channel::clearPwd() {
    this->_pwd.clear();
}

size_t Channel::getUsers(){ return this->_clients.size();}

void    Channel::invite(int fd) {
    this->_invitedClients.push_back(fd);
}
