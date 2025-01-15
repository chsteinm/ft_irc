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
// #include <list>
// #include "Commands.hpp"

// extern int				 exit_state;

#define BUFFER_SIZE 1024

#ifndef DEBUG_MODE
#define DEBUG_MODE 0
#endif

class	Client;
// class   Channel;

static bool exitSIG = false;

class Server {
    private:
        const char*					_password;
		std::vector<pollfd>			_sockets;
		int 						_server_fd;
		std::map<int, Client*>			_clients;
	
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
		// std::string				getPassword();
};

#endif