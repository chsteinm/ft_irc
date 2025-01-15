#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"
// #include "Channel.hpp"

class Client
{
private:
	int _fd;
public:
	Client();
	Client(int fd);
	~Client();

	int		getFd() const;
	// void	setFd(int fd);
};

#endif