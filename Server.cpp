#include "Server.hpp"

void Server::makeNonBlocking(int fd)
{
    fcntl(fd, F_SETFL, O_NONBLOCK);
}

Server::Server(int port, const char* pass) : _password(pass) {
	_listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_listeningSocket < 0) 
        throw std::runtime_error("Socket creation failed");
	int optval = 1;
	setsockopt(_listeningSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
	makeNonBlocking(_listeningSocket);
	sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
	
}