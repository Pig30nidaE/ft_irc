#include "Channel.hpp"


Channel::Channel(std::string name):
channelName(name),capacity(0), index(0), password("") {
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
	//std::cout << "CHECKER1" << std::endl;
	std::vector<User*> &usrList = getUserList();
	//std::cout << "CHECKER2" << std::endl;
	typedef std::vector<User*>::iterator UserIter;
	//std::cout << "CHECKER3" << std::endl;

	for (UserIter iter = usrList.begin();iter != usrList.end();iter++) {
		if ((*iter)->getNickName() == user.getNickName()) {
			//std::cout << "CHECKER4" << std::endl;
			usrList.erase(iter);
			//std::cout << "CHECKER5" << std::endl;
			return ;
		}
	}
	std::cerr << "no such user." << std::endl;
}

void Channel::setChannelName(std::string const channelname) {
	channelName = channelname;
}

std::vector<User*>& Channel::getUserList(void) {
    return userList;
}


std::vector<int>& Channel::getInvitedUserList(void) {
	return invitedUserList;
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

int	Channel::getCapacity(void) {
	return capacity;
}

void Channel::setCapacity(int size) {
	capacity = size;
}