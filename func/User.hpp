#ifndef USER_HPP
#define USER_HPP

#include <vector>
#include <map>
#include <iostream>
#include <iterator>
#include <cstdarg>
#include "Channel.hpp"
#include "ChannelContainer.hpp"
#include "../NumericReplies.hpp"

class Channel;
class ChannelContainer;

class User {
	private:
		std::string							buf;
		std::vector<std::string>			sendBuf;
		int									userSock;
		int									isPass;
		int									userIndex;
		std::string							userName;
		std::string						 	nickName;
		std::string							realName;
		// std::time_t 			connectionTime;
		bool								isOperator;
		//bool					isChannelJoined;
		std::vector<Channel*>				joinedChannelList;
	public:
		User(void);
		~User();

		//void	userRegister(ChannelContainer &channel_container, std::string enteredUserName, std::string enteredNickName);
		//void	nickRegister(Channel& channel, ChannelContainer& channel_contianer);
		void	userInfoErase(User& user); // do I need?
		void	changeUserName(std::string toChange);
		//void	changeNickName(std::string toChange);
		int		joinChannel(std::string toJoinChannel, std::string enterPwd, ChannelContainer& channel_container);
		int		exitChannel(std::string toExitChannel, ChannelContainer& channel_container);
		void	kickUser(std::string toKickUser, Channel& fromChannel, ChannelContainer& channel_container);
		void	inviteUser(User& toInviteUser, Channel& toChannel, ChannelContainer& channel_container);
		void	changeMode(ChannelContainer& channel_container, std::string channel, bool onoff_flag, int type, ...);
		void	topicCommand(ChannelContainer& channel_container, std::string channel, ...);
		// -----donghak----=
		int		checkChannelInList(const std::string &channelName, ChannelContainer& cc);

		std::string							getBuf(void) const;
		void								setBuf(const std::string &buf);
		std::vector<std::string>			getSendBuf(void) const;
		void								setSendBuf(const std::string &buf);
		void								initSendBuf(void);
		std::string 						getUserName(void) const;
		void								setUserName(std::string const username);
		//std::string						 	getNickName(std::string chName);
		std::string							getNickName(void) const;
		void								setNickName(std::string nickname);
		std::string							getRealkName(void) const;
		void								setRealName(std::string realname);
		bool								getIsOperator(void) const;
		void								setIsOperator(bool flag);
		//bool								getIsChannelJoined(void) const;
		//void								setIsChannelJoined(bool flag);
		//int								getJoinedChannelIndex(void) const;
		//void								setJoinedChannelIndex(int const joined_channel_index);
		std::vector<Channel*>				&getJoinedChannelList(ChannelContainer& channel_container);
		Channel								*getSpecificChannelJoined(std::string toGet, ChannelContainer& channel_container);
		int									getUserIndex(void) const;
		void								setUserIndex(int const user_index);
		void 								setUserSock(int sock);
		int 								getUserSock(void) const;
		void 								setUserIsPass(int flag);
		int 								getUserIsPass(void) const;
		class NoJoinedChannelException : public std::exception {
			public:
				const char* what(void) const throw() {
					return "no joined channel.";
				};
		};
};

#endif