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
#include <algorithm>

#include "Client.hpp"
#include "Replies.hpp"
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

static bool exitSIG = false;

struct channel {
		std::string					name;
		std::string					topic;
		std::vector<int>			clientsFd;
		std::vector<int>			operators;
		std::vector<int>			invitedClients;
		std::string					password;
		bool						invitOnly;
		bool						topicOpOnly;
		bool						isPass;
		std::size_t					limit;

		channel() : invitOnly(false), topicOpOnly(false), isPass(false), limit(0) {}
};

class Server {
    private:
		int 						_server_fd;
        const char*					_password;
		std::vector<pollfd>			_sockets;
		std::map<int, Client*>		_clients;
		std::vector<std::string>	_channelsToRemove;
		std::map<std::string, channel>		_channels;
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
		void	removeClientFromAllChannels(int client_fd);
		int		removeClientFromChannel(int client_fd, const std::string& chaName);

		std::vector<std::string>	splitCmd(std::string str);

		Client*	findClientWithNick(std::string nick);
		std::string	returnUserPrefix(Client* client) const;

		void	sendMessageToClient(int client_fd, const std::string &message);
		void	sendMessageToChannel(const std::string &channelName, const std::string &message);

		void	removeEmptyChannels();
		
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
		void	who(Client* client, std::vector<std::string> cmd);
		
		//for join :
		void	names(Client* client, channel& cha);
		void	part(Client* client);
};

#endif