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

Client* Server::findClientWithNick(std::string nick) {
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
        if (it->second->getNick() == nick)
            return it->second;
    return NULL;
}

void    Server::addClient() {
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int client_fd = accept(this->_server_fd, (sockaddr *)&clientAddr, &clientLen);
    if (client_fd == -1) 
    {
        perror("Accept failed");
        return;
    }
    makeNonBlocking(client_fd);
    pollfd pfd = {client_fd, POLLIN, 0};
    this->_sockets.push_back(pfd);
    Client  *client = new Client(client_fd, inet_ntoa(clientAddr.sin_addr));
    this->_clients[client_fd] = client;
    std::cout << "New client connected: " << client_fd << std::endl;
}

void    Server::removeClient(int client_fd) {
    delete this->_clients[client_fd];
    this->_clients.erase(client_fd);
    for (std::vector<pollfd>::iterator it = _sockets.begin(); it != _sockets.end(); ++it) {
        if (it->fd == client_fd) {
            _sockets.erase(it);
            break;
        }
    }
    close(client_fd);
}

std::vector<std::string>	Server::splitCmd(std::string str) {
    std::vector<std::string>    cmd;
    size_t pos, last = 0;
    const char* whitespaces = " \r\t\v\f";

    while ((pos = str.find_first_of(whitespaces, last)) != std::string::npos) {
        cmd.push_back(str.substr(last, pos - last));
        last = pos + 1;
    }
    if (last < str.size()) {
        cmd.push_back(str.substr(last));
    }
	return cmd;
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

    _clients[client_fd]->setBuffer(buffer);
    if (_clients[client_fd]->getBuffer().find_first_of("\r\n") == std::string::npos)
        return;
    std::vector<std::string>    cmd;
    std::istringstream ss(_clients[client_fd]->getBuffer());
    std::string line;
    while (std::getline(ss, line))
    {
        std::vector<std::string>	cmd = splitCmd(line);
        if (!cmd.empty()) {
            if (_cmdMap.find(cmd[0]) != _cmdMap.end()) {
                (this->*_cmdMap[cmd[0]])(_clients[client_fd], cmd);
            } 
            else {
                sendMessageToClient(client_fd, ERR_UNKNOWNCOMMAND(_clients[client_fd]->getNick(), cmd[0]));
            }
        }
    }
    if (DEBUG_MODE)
        std::cout << "\033[36mData receive from client " << client_fd << ":\n" << _clients[client_fd]->getBuffer() << "\033[0m" << std::endl;

    _clients[client_fd]->clearBuff();
}

void    Server::run() {
    if (poll(_sockets.data(), _sockets.size(), -1) == -1 && exitSIG == false)
        throw std::runtime_error("Poll failed");
    for (size_t i = 0; i < this->_sockets.size(); ++i) 
    {
        // if (DEBUG_MODE)
        //     std::cout << "\033[36mi = " << i << "\033[0m" << std::endl;
        if (this->_sockets[i].revents & POLLIN) {
            if (this->_sockets[i].fd == this->_server_fd) {
                addClient();
            } 
            else {
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
    if (listen(_server_fd, SOMAXCON) == -1)
        throw std::runtime_error("Listen failed");
    pollfd pfd = {_server_fd, POLLIN, 0};
    _sockets.push_back(pfd);
    initCmdMap();
}

Server::Server(int port, const char* pass) : _password(pass) {
	init(port);
    signal(SIGINT, &sigHandler);
    std::cout << std::endl << "\tServer initialized on port " << port << "..." << std::endl << std::endl;
    while (exitSIG == false)
        run();
}

void Server::sendMessageToClient(int client_fd, const std::string &message) {
    if (client_fd < 0) {
        std::cerr << "Invalid client_fd: " << client_fd << std::endl;
        return;
    }

    ssize_t bytes_sent = send(client_fd, message.c_str(), message.size(), 0);
    if (bytes_sent <= 0) 
    {
        perror("Send failed");
        if (errno == EAGAIN || errno == EWOULDBLOCK) 
        {
            std::cerr << "Socket send buffer is full. Retry later." << std::endl;
        } 
        else if (errno == ECONNRESET || errno == EPIPE) 
        {
            if (DEBUG_MODE)
                std::cout << "[DEBUG] Client " << client_fd << " disconnected. Removing client." << std::endl;
            removeClient(client_fd);
        }
    } 
    else if (DEBUG_MODE)
    {
        std::cout << "[DEBUG] Sent message to client " << client_fd << ": " << message << std::endl;
    }
}

void    Server::initCmdMap() {
    std::string cmdName[11] = {"CAP", "PASS", "NICK", "USER", "PRIVMSG", "TOPIC", "JOIN", "MODE", "KICK", "INVITE", "QUIT"};
    void (Server::*cmdFunction[11])(Client*, std::vector<std::string>) = {&Server::cap, &Server::pass, &Server::nick, &Server::user, &Server::privmsg, &Server::topic, &Server::join, &Server::mode, &Server::kick, &Server::invite, &Server::quit};

    for (int i = 0; i < 11; ++i) {
        _cmdMap[cmdName[i]] = cmdFunction[i];
    }
}

void    Server::cap(Client* client, std::vector<std::string> args) {
    if (DEBUG_MODE)
        std::cout << "CAP FUNCTION\n";
    (void)client;
    (void)args;
}

void    Server::pass(Client* client, std::vector<std::string> args) {
    if (args.size() < 2)
        sendMessageToClient(client->getFd(), std::string(PREFIX) + ERR_NEEDMOREPARAMS(client->getNick(), args[0]));
    else if (client->getRegisterStatus() > NOT_REGISTERED)
        sendMessageToClient(client->getFd(), std::string(PREFIX) + ERR_ALREADYREGISTRE(client->getNick()));
    else if (args[1] != this->_password)
        sendMessageToClient(client->getFd(), std::string(PREFIX) + ERR_PASSWDMISMATCH(client->getNick()));
    else
        client->setRegister(PASS_OK);
}

bool	checkNick(std::string nick)
{
    if (nick.size() > 9 || isdigit(nick[0])) {
        return false;
    }
	for (std::size_t i = 1; i < nick.size(); i++) {
		if (!isalnum(nick[i]) && nick[i] != '\\' && nick[i] != '|' && nick[i] != '{' && nick[i] != '}' && nick[i] != '[' && nick[i] != ']' && nick[i] != '_')
			return false;
	}
	return true;
}

void    Server::nick(Client* client, std::vector<std::string> args) {
    if (args.size() < 2)
        sendMessageToClient(client->getFd(), std::string(PREFIX) + ERR_NONICKNAMEGIVEN(client->getNick()));
    else if (!checkNick(args[1]))
        sendMessageToClient(client->getFd(), std::string(PREFIX) + ERR_ERRONEUSNICKNAME(args[1]));
    else if (findClientWithNick(args[1]))
        sendMessageToClient(client->getFd(), std::string(PREFIX) + ERR_NICKNAMEINUSE(args[1]));
    else if (client->getRegisterStatus() >= PASS_OK) {
        client->setNick(args[1]);
        client->setRegister(NICK_OK);
    }
}

void    Server::user(Client* client, std::vector<std::string> args) {
    if (args.size() < 5)
        sendMessageToClient(client->getFd(), std::string(PREFIX) + ERR_NEEDMOREPARAMS(client->getNick(), args[0]));
    else if (client->getRegisterStatus() == REGISTERED)
        sendMessageToClient(client->getFd(), std::string(PREFIX) + ERR_ALREADYREGISTRE(client->getNick()));
    client->setUser(args[1]);
	client->setRealName(args[4]);
    if (client->getRegisterStatus() == NICK_OK) {
        client->setRegister(REGISTERED);
        sendMessageToClient(client->getFd(), std::string(PREFIX) + RPL_WELCOME(client->getNick(), args[1], client->getIp()));
    }
}

std::vector<std::string>    split(std::string str, const char* __s) {
    std::vector<std::string>    vec;
    size_t pos, last = 0;

    while ((pos = str.find_first_of(__s, last)) != std::string::npos) {
        vec.push_back(str.substr(last, pos - last));
        last = pos + 1;
    }
    if (last < str.size()) {
        vec.push_back(str.substr(last));
    }
	return vec;
}

void    Server::join(Client* client, std::vector<std::string> args) {
    if (args.size() < 2)
        return sendMessageToClient(client->getFd(), std::string(PREFIX) + ERR_NEEDMOREPARAMS(client->getNick(), args[0]));
    if (args[1] == "0")
        return; //leaveAllChannels
    std::vector<std::string>    channelsNames = split(args[1], ",");
    std::vector<std::string>    keys = (args.size() > 1) ? split(args[2], ",") : std::vector<std::string>();
    for (std::size_t i = 0; i < args.size(); i++) {
        if ((channelsNames[i][0] != '#' && channelsNames[i][0] != '&') || channelsNames[i].size() < 2) {
            sendMessageToClient(client->getFd(), std::string(PREFIX) + ERR_ERRONEUSCHANNEL(client->getNick(), channelsNames[i]));
            continue;
        }
        
    }
}

void    Server::privmsg(Client* client, std::vector<std::string> args) {
    (void)client;
    (void)args;
    
}

void    Server::topic(Client* client, std::vector<std::string> args) {
    (void)client;
    (void)args;
    
}

void    Server::mode(Client* client, std::vector<std::string> args) {
    (void)client;
    (void)args;
    
}

void    Server::kick(Client* client, std::vector<std::string> args) {
    (void)args;
    (void)client;
    
}

void    Server::invite(Client* client, std::vector<std::string> args) {
    (void)args;
    (void)client;
    
}

void    Server::quit(Client* client, std::vector<std::string> args) {
    (void)args;
    (void)client;
    
}

Server::~Server() {
    (_server_fd != -1) && close(_server_fd);
    for (std::vector<pollfd>::iterator it = _sockets.begin(); it != _sockets.end(); it++) 
        close(it->fd);
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
        delete it->second;
}