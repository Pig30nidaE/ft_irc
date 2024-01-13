#ifndef USER_HPP
#define USER_HPP

#include <vector>
#include <map>
#include <iostream>
#include <iterator>
#include <cstdarg>

#include "Channel.hpp"
#include "ChannelContainer.hpp"
#include "NumericReplies.hpp"

class Channel;
class ChannelContainer;

class User {
	private:
		std::string							buf;
		std::vector<std::string>			sendBuf;
		int									userSock;
		int									isPass;
		std::string							userName;
		std::string						 	nickName;
		std::string							realName;
		std::string							hostName;
		std::string							client;
		std::vector<Channel*>				joinedChannelList;
	public:
		User(void);
		~User();

		int		checkChannelInList(const std::string &channelName, ChannelContainer& cc);

		void	changeUserName(std::string toChange);
		int		joinChannel(std::string toJoinChannel, std::string enterPwd, ChannelContainer& channel_container);
		int		exitChannel(std::string toExitChannel, ChannelContainer& channel_container);
		void	kickUser(std::string toKickUser, Channel& fromChannel, ChannelContainer& channel_container);
		void	inviteUser(User& toInviteUser, Channel& toChannel);
		int		changeMode(Channel &channel, bool onoff_flag, int type, std::string arg);
		void	topicCommand(ChannelContainer& channel_container, std::string channel, ...);

		std::string							getBuf(void) const;
		void								setBuf(const std::string &buf);
		std::vector<std::string>			getSendBuf(void) const;
		void								setSendBuf(const std::string &buf);
		void								sendBufClear(void);
		std::string 						getUserName(void) const;
		void								setUserName(std::string const username);
		std::string							getNickName(void) const;
		void								setNickName(std::string nickname);
		std::string							getHostName(void) const;
		void								setHostName(std::string hostname);
		std::string							getRealName(void) const;
		void								setRealName(std::string realname);
		void								setClient(const std::string &nick, const std::string &user, const std::string &host);
		std::string							getClient(void) const;
		std::vector<Channel*>				&getJoinedChannelList(void);
		Channel								*getSpecificChannelJoined(std::string toGet);
		void 								setUserSock(int sock);
		int 								getUserSock(void) const;
		void 								setUserIsPass(int flag);
		int 								getUserIsPass(void) const;
		int									stringToInt(std::string str);
};

#endif