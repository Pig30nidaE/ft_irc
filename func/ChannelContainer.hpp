#ifndef CHANNELCONTAINER_HPP
#define CHANNELCONTAINER_HPP

#include <iostream>
#include <vector>

#include "Channel.hpp"

class ChannelContainer {
	private:
		std::vector<Channel&> channelList;
	public:
		ChannelContainer(void);
		~ChannelContainer();

		void	createChannel(Channel& toCreateChannel);
		void	deleteChannel(Channel& toDeleteChannel);

		std::vector<Channel&> getChannelList(void) const;
};

#endif