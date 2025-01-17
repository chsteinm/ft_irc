#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"
// #include "Channel.hpp"

class Client
{
private:
	int			_fd;
	int			_registered;
	std::string	_nick;
	std::string	_user;
	std::string	_realName;
	std::string	_buffer;
public:
	Client();
	Client(int fd);
	~Client();

	//Getters
	int			getFd() const;
	int			getRegisterStatus() const;
	std::string	getNick() const;
	std::string	getUser() const;
	std::string	getRealName() const;
	std::string	getBuffer() const;

	//Setters
	void	setNick(std::string& nickName);
	void	setRegister(int status);
	void	setUser(std::string& userName);
	void	setRealName(std::string& RealName);
	void	setBuffer(std::string receivedBuff);
	// void	setFd(int fd);
};

#endif