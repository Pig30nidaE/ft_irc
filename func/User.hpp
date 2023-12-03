#ifndef USER_HPP
#define USER_HPP

#include <vector>
#include <iostream>
#include <iterator>
#include "Channel.hpp"
#include "ChannelContainer.hpp"

class Channel;
class ChannelContainer;

class User {
	private:
		int						userIndex;
		std::string				userName;
		//std::string 			nickName;
		// std::time_t 			connectionTime;
		bool					isOperator;
		//bool					isChannelJoined;
		std::vector<Channel*>	joinedChannelList;
	public:
		User(void);
		~User();

		void	userRegister(void);
		void	userInfoErase(User& user);
		void	changeUserName(std::string toChange);
		void	changeNickName(std::string toChange);
		void	joinChannel(std::string toJoinChannel, ChannelContainer& channel_container);
		void	exitChannel(std::string toExitChannel, ChannelContainer& channel_container);
		void	kickUser(User& toKickUser, Channel& fromChannel, ChannelContainer& channel_container);
		void	inviteUser(User& toInviteUser, ChannelContainer& channel_container);
		void	changeMode(int type, ChannelContainer& ChannelContainer);

		std::string 			getUserName(void) const;
		void					setUserName(std::string const username);
		//std::string 			getNickName(void) const;
		//void					setNickName(std::string const nickname);
		bool					getIsOperator(void) const;
		void					setIsOperator(bool flag);
		//bool					getIsChannelJoined(void) const;
		//void					setIsChannelJoined(bool flag);
		//int					getJoinedChannelIndex(void) const;
		//void					setJoinedChannelIndex(int const joined_channel_index);
		std::vector<Channel*>	&getJoinedChannelList(ChannelContainer& channel_container);
		Channel					*getSpecificChannelJoined(std::string toGet, ChannelContainer& channel_container);
		int						getUserIndex(void) const;
		void					setUserIndex(int const user_index);
		class NoJoinedChannelException : public std::exception {
			public:
				const char* what(void) const throw() {
					return "no joined channel.";
				};
		};
};

#endif