#ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include <ctime>
#include <vector>
#include <iterator>

class Channel;
class ChannelContainer;

class User {
	private:
		//-------------------new----------------------
		std::string				msg;
		int						sockNum;
		int						state;

		//-------------------end----------------------
		// int						userIndex;
		// std::string				userName;
		// std::string 			nickName;
		// // std::time_t 			connectionTime;
		// bool					isOperator;
		// bool					isChannelJoined;
	public:
		//-------------------new----------------------
		void		setUserMsg(std::string msg);
		std::string	getUserMsg(void) const;
		
		void		setUserSock(int sockNum);
		int			getUserSock(void) const;

		void		setUserState(int state);
		int			getUserState(void) const;
		//-------------------end----------------------
		User(void);
		~User();
};

#endif