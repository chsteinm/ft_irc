#include "Server.hpp"

void    Server::makeNonBlocking(int fd)
{
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
        throw std::runtime_error("Fcntl failed");
}

void    sigHandler(int sig) {
	if (sig == SIGINT) {
		std::cerr << std::endl << "\tProgram killed" << std::endl;
        exitSIG = true;
    }
}

void    Server::addClient() {
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(this->_server_fd, (sockaddr *)&client_addr, &client_len);
    if (client_fd == -1) 
    {
        perror("Accept failed");
        return;
    }
    makeNonBlocking(client_fd);
    pollfd pfd = {client_fd, POLLIN, 0};
    this->_sockets.push_back(pfd);
    Client  *client = new Client(client_fd);
    this->_clients[client_fd] = client;
    std::cout << "New client connected: " << client_fd << std::endl;
}

void    Server::removeClient(int client_fd) {
    delete this->_clients[client_fd];
    this->_clients.erase(client_fd);
    for (std::vector<pollfd>::iterator it = _sockets.begin(); it != _sockets.end(); it++) {
        if (it->fd == client_fd) {
            _sockets.erase(it);
            break;
        }
    }
    close(client_fd);
}

void    Server::handleReception(int client_fd) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

    if (bytes_received <= 0) {
        std::cout << "Client " << client_fd << " disconnected." << std::endl;
        removeClient(client_fd);
        return;
    }

    if (DEBUG_MODE)
        std::cout << "\033[36mData receive from client " << client_fd << ":\n" << buffer << "\033[0m" << std::endl;
}

void    Server::run() {
    if (poll(_sockets.data(), _sockets.size(), -1) == -1 && exitSIG == false)
        throw std::runtime_error("Poll failed");
    for (size_t i = 0; i < this->_sockets.size(); ++i) 
    {
        if (this->_sockets[i].revents & POLLIN) 
        {
            if (this->_sockets[i].fd == this->_server_fd) 
            {
                addClient();
            } 
            else 
            {
                handleReception(this->_sockets[i].fd);
            }
        }
    }
}

void    Server::init(int port) {
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd < 0) {
        throw std::runtime_error("Socket creation failed");
    }
	int optval = 1;
	setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
	makeNonBlocking(_server_fd);
	sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
	if (bind(_server_fd, (sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        throw std::runtime_error("Bind failed");
    if (listen(_server_fd, SOMAXCONN) == -1)
        throw std::runtime_error("Listen failed");
    pollfd pfd = {_server_fd, POLLIN, 0};
    _sockets.push_back(pfd);
}

Server::Server(int port, const char* pass) : _password(pass) {
	init(port);
    signal(SIGINT, &sigHandler);
    std::cout << std::endl << "\tServer initialized on port " << port << "..." << std::endl << std::endl;
    while (exitSIG == false)
        run();
}

Server::~Server() {
    (_server_fd != -1) && close(_server_fd);
    for (std::vector<pollfd>::iterator it = _sockets.begin(); it != _sockets.end(); it++) 
        close(it->fd);
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
        delete it->second;
}