#ifndef	REPLIES_HPP

#define REPLIES_HPP

#define PREFIX ":42FinalRush.com" 

#define ERR_NEEDMOREPARAMS(nick, command)		" 461 " + nick + " " + command + " :Not enough parameters\r\n"
#define ERR_UNKNOWNCOMMAND(nick, command)		" 421 " + nick + " " + command + " :Unknown command\r\n"
#define ERR_ALREADYREGISTRE(nick)				" 462 " + nick + " :Unauthorized command (already registered)\r\n"
#define ERR_PASSWDMISMATCH(nick)				" 464 " + nick + " :Password incorrect\r\n"
#define ERR_NONICKNAMEGIVEN(nick)				" 431 " + nick + " :No nickname given\r\n"
#define ERR_ERRONEUSNICKNAME(nick)				" 432 " + nick + " " + nick + " :Erroneous nickname\r\n"
#define ERR_NICKNAMEINUSE(nick)					" 433 " + nick + " " + nick + " :Nickname is already in use\r\n"
#define RPL_WELCOME(nick, user, host)			" 001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host + "\r\n"
#define ERR_ERRONEUSCHANNEL(nick, chaName)		" 600 " + nick + " " + chaName + " :Erroneous channel name (usage: #/&<channelname>)\r\n"


#endif