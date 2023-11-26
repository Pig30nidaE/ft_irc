#include "Channel.hpp"


Channel::Channel(std::string name, std::string topic_contents, int type):
topic(topic_contents), mode(type), channelName(name) {}

Channel::~Channel() {}

std::string Channel::getChannelName(void) const {
	return channelName;
};

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
	int to_del_idx = *to_del_usr - *(getUserList().begin());
	userList.erase(getUserList().begin() + to_del_idx);
}

void Channel::setChannelName(std::string const channelname) {
	channelName = channelname;
}

std::vector<User*> Channel::getUserList(void) const {
	return userList;	
}

Topic Channel::getTopicRef(void) const {
	return topic;
}

Mode Channel::getModeRef(void) const {
	return mode;
}