#include "ChannelContainer.hpp"

ChannelContainer::ChannelContainer(void) {}

ChannelContainer::~ChannelContainer() {}

void ChannelContainer::createChannel(Channel& toCreateChannel) {
	channelList.push_back(&toCreateChannel);
}

void ChannelContainer::deleteChannel(Channel& toDeleteChannel) {
	std::vector<Channel*>::iterator to_del_cha = find(getChannelList().begin(), getChannelList().end(),\
		 &toDeleteChannel);
	if (to_del_cha == getChannelList().end()) {
		std::cerr << "no such channel." << std::endl;
		return ;
	}
	int to_del_idx = *to_del_cha - *(getChannelList().begin());
	getChannelList().erase(getChannelList().begin() + to_del_idx);
}

std::vector<Channel*> ChannelContainer::getChannelList(void) const {
	return channelList;
}