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
		int						userJoinChanLimit;
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
		int						getUserJoinChanLimit(void) const;
		void					setUserJoinChanLimit(const int limit);
		// donghak
		int						checkClientInList(const std::string &nickname);
		User					*getUser(const std::string &nickname);
};

#endif