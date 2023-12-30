#ifndef CHANNELCONTAINER_HPP
#define CHANNELCONTAINER_HPP

#include <vector>
#include "Channel.hpp"

class Channel;
class User;
class ChannelContainer {
	private:
		std::vector<Channel*> 	channelList;
		std::vector<User*>		userList;
		int						channelIndex;
	public:
		ChannelContainer(void);
		~ChannelContainer();

		void	createChannel(Channel& toCreateChannel);
		void	deleteChannel(Channel& toDeleteChannel);
		//bool	checkUserName(std::string toFind);
		bool	checkNickName(std::string toFind);

		std::vector<Channel*> 	&getChannelList(void);
		std::vector<User*>		&getUserList(void);
		int						getCurrentChIdx(void);
		void					setCurrentChIdx(int idx);
		// donghak
		int						checkClientInList(const std::string &nickname);
		User					*getUser(const std::string &nickname);
};

#endif