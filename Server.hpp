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
// #include "Client.hpp"
// #include <list>
// #include "Commands.hpp"

// extern int				 exit_state;

class	Client;
class   Channel;

class Server {
    private:
        const char*					_password;
		std::map<int, Client *>		_clientList;
		std::vector<pollfd>			_sockets;
		int 						_listeningSocket;

        int								flags;
        struct pollfd					sockets[SOMAXCONN]; //listening socket is sockets[0]
        int								socket_number;
        int								events_number;
        void							initServer();
        std::vector<pollfd>::iterator	new_connection();
        std::vector<pollfd>::iterator	disconnect(int fd);
		
        std::vector<Channel>			_Channels;
	
    public:

        Server();
        ~Server();
        Server &operator=(Server const &a);
        Server(int port, const char *password);

		void 	makeNonBlocking(int fd);
		void	monitoring();
		int 	shut_down();
		void	adduser(int fd, std::string hostname);
		void	command_handler(Client *client, std::vector<std::string> args);
		void	sig_handler(int);
		bool	handle_data(std::vector<pollfd>::iterator it);
		void	errorin(bool err, const char *msg);

		Client	*find_client(std::string nickname);
		Channel	*find_channel(std::string channel_name);
		void	del_channel( std::string channel_name );
		void	quit_channels( Client *client, Server &serv );

		void	broadcast_server(std::string message);

	// SETTER

		void					addChannels(Channel channel);

	// GETTER

		std::vector<Channel>	getChannels();
		std::string				getPassword();
};

#endif