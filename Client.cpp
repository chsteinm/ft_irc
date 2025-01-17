#include "Client.hpp"

Client::Client()
{
	_fd = -1;
}

Client::Client(int fd) {
	_fd = fd;
	_registered = false;
}

Client::~Client(){}


//Getters

int		Client::getFd() const {
	return _fd;
}

int		Client::getRegisterStatus() const {
	return _registered;
}

std::string	Client::getNick() const {
	return _nick;
}

std::string	Client::getUser() const {
	return _user;
}

std::string	Client::getBuffer() const {
	return _buffer;
}


//Setters

void	Client::setRegister(int status) {
	_registered = status;
}

void	Client::setNick(std::string& nickName) {
	_nick = nickName;
}

void	Client::setUser(std::string& userName) {
	_user = userName;
}

void	Client::setRealName(std::string& realName) {
	_realName = realName;
}

void	Client::setBuffer(std::string recievedBuff) {
	_buffer += recievedBuff;
}

// void	Client::setFd(int fd) {
// 	_fd = fd;
// }