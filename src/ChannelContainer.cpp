#include "../include/ChannelContainer.hpp"

ChannelContainer::ChannelContainer(void):userJoinChanLimit(5) {}

ChannelContainer::~ChannelContainer() {}

void ChannelContainer::createChannel(Channel& toCreateChannel) {
	channelList.push_back(&toCreateChannel);
}

void ChannelContainer::deleteChannel(Channel& toDeleteChannel) {
	typedef std::vector<Channel*>::iterator ChIter;
	for (ChIter iter = channelList.begin();iter != channelList.end();iter++) {
		if ((*iter)->getChannelName() == toDeleteChannel.getChannelName()) {
			channelList.erase(iter);
			break ;
		}
	}
	delete &toDeleteChannel;
}

bool ChannelContainer::checkNickName(std::string toFind) {
	typedef std::vector<User*>::iterator UserIterator;
	UserIterator user_iter;
	for (user_iter = getUserList().begin();user_iter != getUserList().end();user_iter++) {
		if ((*user_iter)->getNickName() == toFind) {
			return true;
		}
	}
	return false;
}

Channel* ChannelContainer::getSpecificChannelByName(std::string channel) {
	std::vector<Channel*>::iterator ch_iter = getChannelList().begin();

	for (ch_iter = getChannelList().begin();\
		ch_iter != getChannelList().end();ch_iter++) {
			if ((*ch_iter)->getChannelName() == channel)
				return *ch_iter;
		}
	return NULL;
}

std::vector<Channel*> &ChannelContainer::getChannelList(void) {
	return channelList;
}

std::vector<User*> &ChannelContainer::getUserList(void) {
	return userList;
}

int ChannelContainer::getUserJoinChanLimit(void) const {
	return userJoinChanLimit;
}

void ChannelContainer::setUserJoinChanLimit(const int limit) {
	userJoinChanLimit = limit;
}

int ChannelContainer::checkClientInList(const std::string &nickname) {
	for (std::vector<User *>::iterator iter = userList.begin(); iter != userList.end(); iter++) {
		if ((*iter)->getNickName() == nickname) {
			return 0;
		}
	}
	return ERR_NOSUCHNICK_NUM;
}

User *ChannelContainer::getUser(const std::string &nickname) {
	for (std::vector<User *>::iterator iter = userList.begin(); iter != userList.end(); iter++) {
		if ((*iter)->getNickName() == nickname) {
			return (*iter);
		}
	}
	return NULL;
}