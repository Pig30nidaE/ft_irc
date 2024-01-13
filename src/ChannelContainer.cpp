#include "../include/ChannelContainer.hpp"

ChannelContainer::ChannelContainer(void):channelIndex(-1),userJoinChanLimit(5) {}

ChannelContainer::~ChannelContainer() {}

void ChannelContainer::createChannel(Channel& toCreateChannel) {
	setCurrentChIdx(getCurrentChIdx() + 1);
	toCreateChannel.setIndex(getCurrentChIdx());
	channelList.push_back(&toCreateChannel);
}

void ChannelContainer::deleteChannel(Channel& toDeleteChannel) {
	std::vector<Channel*>::iterator to_del_cha = find(getChannelList().begin(), getChannelList().end(),\
		 &toDeleteChannel);
	if (to_del_cha == getChannelList().end()) {
		return ;
	}
	int to_del_idx = (*to_del_cha)->getIndex() - getChannelList().at(0)->getIndex();
	getChannelList().erase(getChannelList().begin() + to_del_idx);
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

int	ChannelContainer::getCurrentChIdx(void) {
	return channelIndex;
}

void ChannelContainer::setCurrentChIdx(int idx) {
	channelIndex = idx;
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