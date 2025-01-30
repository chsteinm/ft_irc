#ifndef	REPLIES_HPP

#define REPLIES_HPP

#define PREFIX ":42FinalRush.com" 

#define ERR_NOSUCHCHANNEL(nick, chaName)		" 403 " + nick + " " + chaName + " :No such channel\r\n"
#define ERR_UNKNOWNCOMMAND(nick, command)		" 421 " + nick + " " + command + " :Unknown command\r\n"
#define ERR_NONICKNAMEGIVEN(nick)				" 431 " + nick + " :No nickname given\r\n"
#define ERR_ERRONEUSNICKNAME(nick)				" 432 " + nick + " " + nick + " :Erroneous nickname\r\n"
#define ERR_NICKNAMEINUSE(nick)					" 433 " + nick + " " + nick + " :Nickname is already in use\r\n"
#define ERR_USERNOTINCHANNEL(nick, chaName)		" 441 " + nick + " " + nick + " " + chaName + " :They aren't on that channel\r\n"
#define ERR_NEEDMOREPARAMS(nick, command)		" 461 " + nick + " " + command + " :Not enough parameters\r\n"
#define ERR_ALREADYREGISTRE(nick)				" 462 " + nick + " :Unauthorized command (already registered)\r\n"
#define ERR_PASSWDMISMATCH(nick)				" 464 " + nick + " :Password incorrect\r\n"
#define ERR_CHANNELISFULL(nick, chaName)		" 471 " + nick + " " + chaName + " :Cannot join channel (+l)\r\n"
#define ERR_INVITEONLYCHAN(nick, chaName)		" 473 " + nick + " " + chaName + " :Cannot join channel (+i)\r\n"
#define ERR_BADCHANNELKEY(nick, chaName)		" 475 " + nick + " " + chaName + " :Cannot join channel (+k)\r\n"
#define ERR_ERRONEUSCHANNEL(nick, chaName)		" 476 " + nick + " " + chaName + " :Erroneous channel name (usage: #/&<channelname>)\r\n"
#define ERR_CHANOPRIVSNEEDED(nick, chaName)		" 482 " + nick + " " + chaName + " :You're not channel operator\r\n"

#define RPL_WELCOME(nick, user, host)			" 001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host + "\r\n"
#define RPL_NOTOPIC(nick, chaName)				" 331 " + nick + " " + chaName + " :No topic is set\r\n"
#define RPL_TOPIC(nick, chaName, topic)			" 332 " + nick + " " + chaName + " :" + topic + "\r\n"
#define RPL_NAMREPLY(nick, chaName, nameList)	" 353 " + nick + " = " + chaName + " :" + nameList + "\r\n"
#define RPL_ENDOFNAMES(nick, chaName)			" 366 " + nick + " " + chaName + " :End of NAMES list\r\n"

#define JOIN(nick, user, host, chaName)			":" + nick + "!" + user + "@" + host + " JOIN " + chaName + "\r\n"
#define PART(nick, user, host, chaName)			":" + nick + "!" + user + "@" + host + " PART " + chaName + " :" + nick + "\r\n"

#endif