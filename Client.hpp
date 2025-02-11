#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"

class Client {
	private:
		int			_fd;
		int			_registered;
		std::string	_host;
		std::string	_nick;
		std::string	_user;
		std::string	_realName;
		std::string	_buffer;
		bool		_disconnected;
	public:
		Client();
		Client(int fd, std::string clientIP);
		~Client();

		//Getters
		int			getFd() const;
		int			getRegisterStatus() const;
		std::string	getNick() const;
		std::string	getUser() const;
		std::string	getRealName() const;
		std::string	getBuffer() const;
		std::string	getHost() const;
		bool		getDisconnected() const;

		//Setters
		void	setNick(std::string& nickName);
		void	setRegister(int status);
		void	setUser(std::string& userName);
		void	setRealName(std::string& RealName);
		void	setBuffer(std::string receivedBuff);
		void	setDisconnected(bool status);

		void	clearBuff();
};

#endif