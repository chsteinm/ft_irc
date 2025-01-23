#ifndef	SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <sstream>
#include <sys/poll.h>
#include <sys/types.h> 
#include <sys/socket.h>				 
#include <poll.h>
#include <map>
#include <vector>
#include <signal.h> 
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "Client.hpp"
#include "Replies.hpp"
#include "Channel.hpp"
// #include <list>
// #include "Commands.hpp"

// extern int				 exit_state;

#define BUFFER_SIZE 512
#define SOMAXCON 10

#define NOT_REGISTERED 0
#define PASS_OK 1
#define NICK_OK 2
#define REGISTERED 3

#ifndef DEBUG_MODE
#define DEBUG_MODE 0
#endif

class	Client;
class   Channel;

static bool exitSIG = false;

class Server {
    private:
        const char*					_password;
		std::vector<pollfd>			_sockets;
		int 						_server_fd;
		std::map<int, Client*>		_clients;
		std::map<int, Channel*>		_channels;
		std::map<std::string, void (Server::*)(Client*, std::vector<std::string>)> _cmdMap;
	
    public:
        Server();
        ~Server();
        Server(int port, const char *password);

		void	init(int port);
		void 	makeNonBlocking(int fd);
		void	run();
		void	addClient();
		void	handleReception(int client_fd);
		void	removeClient(int client_fd);

		std::vector<std::string>	splitCmd(std::string str);

		Client*	findClientWithNick(std::string nick);

		void	sendMessageToClient(int client_fd, const std::string &message);
		
		void	initCmdMap();
		void	cap(Client* client, std::vector<std::string> cmd);
		void	pass(Client* client, std::vector<std::string> cmd);
		void	nick(Client* client, std::vector<std::string> cmd);
		void	user(Client* client, std::vector<std::string> cmd);
		void	privmsg(Client* client, std::vector<std::string> cmd);
		void	topic(Client* client, std::vector<std::string> cmd);
		void	join(Client* client, std::vector<std::string> cmd);
		void	mode(Client* client, std::vector<std::string> cmd);
		void	kick(Client* client, std::vector<std::string> cmd);
		void	invite(Client* client, std::vector<std::string> cmd);
		void	quit(Client* client, std::vector<std::string> cmd);
};

#endif