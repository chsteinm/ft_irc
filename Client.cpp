#include "Client.hpp"

Client::Client()
{
	_fd = -1;
}

Client::Client(int fd) {
	this->_fd = fd;
}

Client::~Client(){}

int		Client::getFd() const {
	return _fd;
}

// void	Client::setFd(int fd) {
// 	_fd = fd;
// }