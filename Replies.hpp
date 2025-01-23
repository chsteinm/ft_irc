#ifndef	REPLIES_HPP

#define REPLIES_HPP

#define ERR_NEEDMOREPARAMS(nick, command)		":42FinalRush.com 461 " + nick + " " + command + " :Not enough parameters\r\n"
#define ERR_UNKNOWNCOMMAND(nick, command)		":42FinalRush.com 421 " + nick + " " + command + " :Unknown command\r\n"
#define ERR_ALREADYREGISTRE(nick)				":42FinalRush.com 462 " + nick + " :Unauthorized command (already registered)\r\n"
#define ERR_PASSWDMISMATCH(nick)				":42FinalRush.com 464 " + nick + " :Password incorrect\r\n"
#define ERR_NONICKNAMEGIVEN(nick)				":42FinalRush.com 431 " + nick + " :No nickname given\r\n"
#define ERR_ERRONEUSNICKNAME(nick)				":42FinalRush.com 432 " + nick + " " + nick + " :Erroneous nickname\r\n"
#define ERR_NICKNAMEINUSE(nick)					":42FinalRush.com 433 " + nick + " " + nick + " :Nickname is already in use\r\n"
#define RPL_WELCOME(nick, user, host)			":42FinalRush.com 001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host + "\r\n"

#endif