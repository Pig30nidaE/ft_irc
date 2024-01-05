#include "ChannelContainer.hpp"

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
		std::cerr << "no such channel." << std::endl;
		return ;
	}
	int to_del_idx = *to_del_cha - *(getChannelList().begin());
	getChannelList().erase(getChannelList().begin() + to_del_idx);
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

// 동학
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