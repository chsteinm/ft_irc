#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "Server.hpp"

class Server;
class Client;

class Channel {
	private:
		std::string					_name;
		std::string					_topic;
		std::vector<std::string>	_channelClients;
		std::vector<std::string>	_operators;
		std::vector<std::string>	_invitedClients;
		std::string					_password;

	public:
		Channel(std::string name);
		~Channel();

// // SETTER
// 		void						set_invite_only( int i );
// 		void						set_channel_key( int i, std::string password );
// 		void						set_user_limit( int limit );
// 		void						set_restriction_TOPIC_cmd( int i );
// 		void						set_topic( std::string name );
// 		void						set_primo( std::string new_primo );
// 		void						add_operator( std::string client );
// 		void						add_invite( std::string invite );

// // GETTER

// 		std::string					get_name();
		// Server						*get_server();
		// std::vector<std::string>	get_channelClients();
		// std::vector<std::string>	get_operators();
		// std::string					get_topic();
		// std::vector<std::string>	get_invite();

		// bool						get_invite_only();
		// bool						get_channel_key();
		// std::string					get_password();
		// bool						get_user_limit();
		// int							get_user_limit_nb();
		// bool						get_restriction_TOPIC_cmd();

};

#endif