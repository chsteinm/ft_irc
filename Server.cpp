#include "Server.hpp"

void    Server::makeNonBlocking(int fd)
{
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
        throw std::runtime_error("Fcntl failed");
}

bool isClient(const std::vector<int>& vec, int clientFd) {
    return std::find(vec.begin(), vec.end(), clientFd) != vec.end();
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

int    eraseFdInVector(std::vector<int>& vec, int fd) {
    for (std::vector<int>::iterator it = vec.begin(); it != vec.end(); ++it) {
        if (*it == fd) {
            vec.erase(it);
            return 1;
        }
    }
    return 0;
}

void    Server::removeEmptyChannels() {
    for (std::vector<std::string>::iterator it = _channelsToRemove.begin(); it != _channelsToRemove.end(); it++)
        _channels.erase(*it);
    _channelsToRemove.clear();
}

int    Server::removeClientFromChannel(int client_fd, const std::string& chaName) {
    std::map<std::string, channel>::iterator it = _channels.find(chaName);
    if (it == _channels.end())
        return 0;
    eraseFdInVector(it->second.operators, client_fd);
    if (eraseFdInVector(it->second.clientsFd, client_fd)) {
        if (it->second.clientsFd.empty())
            _channelsToRemove.push_back(chaName);
        return 1;
    }
    return 0;
}

// void    Server::removeClientFromAllChannels(int client_fd) {
//     for (std::map<std::string, channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
//         removeClientFromChannel(client_fd, it->second.name);
//     }
// }

void    Server::part(Client* client) {
    for (std::map<std::string, channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        if (removeClientFromChannel(client->getFd(), it->second.name)) {
            sendMessageToChannel(it->second.name, PART(client->getNick(), client->getUser(), client->getIp(), it->second.name));
            sendMessageToClient(client->getFd(), PART(client->getNick(), client->getUser(), client->getIp(), it->second.name));
        }
    }
    removeEmptyChannels();
}

void    Server::removeClient(int client_fd) {
    part(_clients[client_fd]);
    delete this->_clients[client_fd];
    this->_clients.erase(client_fd);
    for (std::vector<pollfd>::iterator it = _sockets.begin(); it != _sockets.end(); ++it) {
        if (it->fd == client_fd) {
            _sockets.erase(it);
            break;
        }
    }
    // removeClientFromAllChannels(client_fd);
    close(client_fd);
    removeEmptyChannels();
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
        _clients[client_fd]->setDisconnected(true);
        removeClient(client_fd);
        return;
    }

    if (DEBUG_MODE)
        std::cout << "\033[36mData receive from client " << client_fd << ":\n" << buffer << "\033[0m" << std::endl;

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
                sendMessageToClient(client_fd, std::string(PREFIX) + ERR_UNKNOWNCOMMAND(_clients[client_fd]->getNick(), cmd[0]));
            }
        }
    }
    _clients[client_fd]->clearBuff();
}

void    Server::run() {
    if (poll(_sockets.data(), _sockets.size(), -1) == -1 && exitSIG == false)
        throw std::runtime_error("Poll failed");
    for (size_t i = 0; i < this->_sockets.size(); ++i) 
    {
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
    if (_clients[client_fd]->getDisconnected() == true)
        return;
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

void    Server::sendMessageToChannel(const std::string &channelName, const std::string &message) {
    std::map<std::string, channel>::iterator it = _channels.find(channelName);
    if (it != _channels.end()) {
        for (std::size_t i = 0; i < it->second.clientsFd.size(); i++) {
            sendMessageToClient(it->second.clientsFd[i], message);
        }
    }
}

void    Server::initCmdMap() {
    std::string cmdName[12] = {"CAP", "PASS", "NICK", "USER", "PRIVMSG", "TOPIC", "JOIN", "MODE", "KICK", "INVITE", "QUIT", "WHO"};
    void (Server::*cmdFunction[12])(Client*, std::vector<std::string>) = {&Server::cap, &Server::pass, &Server::nick, &Server::user, &Server::privmsg, &Server::topic, &Server::join, &Server::mode, &Server::kick, &Server::invite, &Server::quit, &Server::who};

    for (int i = 0; i < 12; ++i) {
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

void    Server::names(Client* client, channel& cha) {
    std::ostringstream nameList;
    for (std::vector<int>::iterator it = cha.clientsFd.begin(); it != cha.clientsFd.end(); ++it) {
        if (isClient(cha.clientsFd, *it))
            nameList << "@" << _clients[*it]->getNick() << " ";
        else
            nameList << _clients[*it]->getNick() << " ";
    }
    sendMessageToClient(client->getFd(), std::string(PREFIX) + RPL_NAMREPLY(client->getNick(), cha.name, nameList.str()));
    sendMessageToClient(client->getFd(), std::string(PREFIX) + RPL_ENDOFNAMES(client->getNick(), cha.name));
}

void    Server::join(Client* client, std::vector<std::string> args) {
    if (args.size() < 2)
        return sendMessageToClient(client->getFd(), std::string(PREFIX) + ERR_NEEDMOREPARAMS(client->getNick(), args[0]));
    if (args[1] == "0")
        return part(client); //leaveAllChannels
    std::vector<std::string>    channelsNames = split(args[1], ",");
    std::vector<std::string>    keys = (args.size() > 2) ? split(args[2], ",") : std::vector<std::string>();
    for (std::size_t i = 0; i < channelsNames.size(); i++) {
        if ((channelsNames[i][0] != '#' && channelsNames[i][0] != '&') || channelsNames[i].size() < 2) {
            sendMessageToClient(client->getFd(), std::string(PREFIX) + ERR_ERRONEUSCHANNEL(client->getNick(), channelsNames[i]));
            continue;
        }
        if (_channels.find(channelsNames[i]) == _channels.end()) {
            channel newChannel;
            newChannel.clientsFd.push_back(client->getFd());
            newChannel.operators.push_back(client->getFd());
            if ((newChannel.isPass = (keys.size() > i)))
                newChannel.password = keys[i];
            newChannel.name = channelsNames[i];
            _channels[channelsNames[i]] = newChannel;
        }
        else {
            channel& cha = _channels[channelsNames[i]];
            if (cha.invitOnly) {
                if (!isClient(cha.invitedClients, client->getFd())) {
                    sendMessageToClient(client->getFd(), std::string(PREFIX) + ERR_INVITEONLYCHAN(client->getNick(), cha.name));
                    continue;
                }
            }
            if (cha.limit && cha.clientsFd.size() >= cha.limit) {
                sendMessageToClient(client->getFd(), std::string(PREFIX) + ERR_INVITEONLYCHAN(client->getNick(), cha.name));
                continue;
            }
            if (cha.isPass && (keys.size() < i + 1 || keys[i] != cha.password)) {
                sendMessageToClient(client->getFd(), std::string(PREFIX) + ERR_BADCHANNELKEY(client->getNick(), cha.name));
                continue;
            }
            
        }
        sendMessageToChannel(channelsNames[i], JOIN(client->getNick(), client->getUser(), client->getIp(), channelsNames[i]));
        if (_channels[channelsNames[i]].topic.empty())
            sendMessageToClient(client->getFd(), std::string(PREFIX) + RPL_NOTOPIC(client->getNick(), channelsNames[i]));
        else
            sendMessageToClient(client->getFd(), std::string(PREFIX) + RPL_TOPIC(client->getNick(), channelsNames[i], _channels[channelsNames[i]].topic));
        names(client, _channels[channelsNames[i]]);
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
    if (args.size() < 2)
        return sendMessageToClient(client->getFd(), std::string(PREFIX) + ERR_NEEDMOREPARAMS(client->getNick(), args[0]));
    std::map<std::string, channel>::iterator itCha = _channels.find(args[1]);
    if (itCha == _channels.end())
        return sendMessageToClient(client->getFd(), std::string(PREFIX) + ERR_NOSUCHCHANNEL(client->getNick(), args[1]));
    channel cha = itCha->second;
    if (!isClient(cha.operators, client->getFd()))
        return sendMessageToClient(client->getFd(), std::string(PREFIX) + ERR_CHANOPRIVSNEEDED(client->getNick(), args[1]));
    // if (!isClient(cha.clientsFd, client->getFd()))
    //     return sendMessageToClient(client->getFd(), std::string(PREFIX) + ERR_USERNOTINCHANNEL(client->getNick(), args[1]));

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

void    Server::who(Client* client, std::vector<std::string> args) {
    if (args.size() < 2)
        return sendMessageToClient(client->getFd(), std::string(PREFIX) + ERR_NEEDMOREPARAMS(client->getNick(), args[0]));
    if (_channels.find(args[1]) == _channels.end())
        return sendMessageToClient(client->getFd(), std::string(PREFIX) + ERR_NOSUCHCHANNEL(client->getNick(), args[1]));
}

Server::~Server() {
    (_server_fd != -1) && close(_server_fd);
    for (std::vector<pollfd>::iterator it = _sockets.begin(); it != _sockets.end(); it++) 
        close(it->fd);
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
        delete it->second;
}