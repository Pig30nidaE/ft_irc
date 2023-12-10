#include "Channel.hpp"


Channel::Channel(std::string name):
channelName(name),capcity(0) {
	mode = new Mode(ALL_MODES_OFF);
	topic = new Topic("");
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
	//user.getNickName().erase(getChannelName());
}

bool Channel::isUserBan(int userSock) {
	if (std::find(bannedUserList.begin(), bannedUserList.end(), userSock) != bannedUserList.end())
		return true;
	return false;
}

void Channel::setChannelName(std::string const channelname) {
	channelName = channelname;
}

std::vector<User*>& Channel::getUserList(void) {
    return userList;
}

std::vector<int>& Channel::getBannedUserList(void) {
	return bannedUserList;
}

std::string Channel::getPassword(void) const {
	return password;
}

void Channel::setPassword(std::string pwd) {
	password = pwd;
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

int	Channel::getCapcity(void) {
	return capcity;
}

void Channel::setCapcity(int size) {
	capcity = size;
}