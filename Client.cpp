#include "Client.hpp"

Client::Client()
{
	_fd = -1;
}

Client::Client(int fd, std::string ip) : _fd(fd), _ip(ip) {
	_registered = NOT_REGISTERED;
	_disconnected = false;
}

Client::~Client(){}


//Getters

int		Client::getFd() const {
	return _fd;
}

int		Client::getRegisterStatus() const {
	return _registered;
}

bool	Client::getDisconnected() const {
	return _disconnected;
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

std::string	Client::getIp() const {
	return _ip;
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

void	Client::setDisconnected(bool status) {
	_disconnected = status;
}

void	Client::clearBuff() {
		_buffer.clear();
}


// void	Client::setFd(int fd) {
// 	_fd = fd;
// }