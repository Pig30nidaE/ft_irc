#include "Channel.hpp"


Channel::Channel(std::string name, std::string topic_contents, int type):
channelName(name) {
	topic = new Topic(topic_contents);
	mode = new Mode(type);
}

Channel::~Channel() {
	delete topic;
	delete mode;
}

std::string Channel::getChannelName(void) const {
	return channelName;
}

void Channel::userAdd(User& user) {
	userList.push_back(&user);
}

void Channel::userDel(User& user) {
	std::vector<User*>::iterator to_del_usr = find(getUserList().begin(), getUserList().end(),\
		 &user);
	if (to_del_usr == getUserList().end()) {
		std::cerr << "no such user." << std::endl;
		return ;
	}
	//int to_del_idx = *to_del_usr - *(getUserList().begin());
	userList.erase(getUserList().begin() + (*to_del_usr)->getUserIndex());
	user.getNickName().erase(getChannelName());
}

void Channel::setChannelName(std::string const channelname) {
	channelName = channelname;
}

std::vector<User*>& Channel::getUserList(void) {
    return userList;
}


Topic& Channel::getTopicRef(void) {
	return *topic;
}

Mode& Channel::getModeRef(void) {
	return *mode;
}

int Channel::getIndex(void) {
	return index;
}

void Channel::setIndex(int idx) {
	index = idx;
}