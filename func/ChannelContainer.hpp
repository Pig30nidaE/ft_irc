#ifndef CHANNELCONTAINER_HPP
#define CHANNELCONTAINER_HPP

#include <vector>
#include "Channel.hpp"

class Channel;
class ChannelContainer {
	private:
		std::vector<Channel*> 	channelList;
		std::vector<User*>		userNameList;
		int						channelIndex;
	public:
		ChannelContainer(void);
		~ChannelContainer();

		void	createChannel(Channel& toCreateChannel);
		void	deleteChannel(Channel& toDeleteChannel);

		std::vector<Channel*> 	&getChannelList(void);
		int						getCurrentChIdx(void);
		void					setCurrentChIdx(int idx);
};

#endif