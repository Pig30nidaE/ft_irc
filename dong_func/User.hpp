#ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include <ctime>
#include <vector>
#include <iterator>

#include "Channel.hpp"
#include "ChannelContainer.hpp"

class User {
	private:
		//-------------------new----------------------
		std::string				msg;
		int						sockNum;
		int						state;

		//--------------------------------------------
		int						userIndex;
		std::string				userName;
		std::string 			nickName;
		// std::time_t 			connectionTime;
		bool					isOperator;
		bool					isChannelJoined;
		int						joinedChannelIndex;
	public:
		//---------------------new--------------------------
		void		setUserMsg(std::string msg);
		std::string	getUserMsg(void) const;
		
		void		setUserSock(int sockNum);
		int			getUserSock(void) const;

		void		setUserState(int state);
		int			getUserState(void) const;
		//---------------------------------------------------
		User(void);
		~User();

		void	userRegister(void);
		void	userInfoErase(User& user);
		void	changeUserName(std::string toChange);
		void	changeNickName(std::string toChange);
		void	joinChannel(std::string toJoinChannel, ChannelContainer& channel_container);
		void	exitChannel(ChannelContainer& channel_container);
		void	kickUser(User& toKickUser, ChannelContainer& channel_container);
		void	inviteUser(User& toInviteUser, ChannelContainer& channel_container);

		std::string getUserName(void) const;
		void		setUserName(std::string const username);
		std::string getNickName(void) const;
		void		setNickName(std::string const nickname);
		bool		getIsOperator(void) const;
		void		setIsOperator(bool flag);
		bool		getIsChannelJoined(void) const;
		void		setIsChannelJoined(bool flag);
		int			getJoinedChannelIndex(void) const;
		void		setJoinedChannelIndex(int const joined_channel_index);
		Channel&	getJoinedChannel(ChannelContainer& channer_container) const;
		int			getUserIndex(void) const;
		void		setUserIndex(int const user_index);
};

#endif