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
	std::vector<User*> &usrList = getUserList();
	typedef std::vector<User*>::iterator UserIter;

	for (UserIter iter = usrList.begin();iter != usrList.end();iter++) {
		if ((*iter)->getNickName() == user.getNickName()) {
			usrList.erase(iter);
			return ;
		}
	}
	std::cerr << "no such user." << std::endl;
}

void Channel::operatorAdd(User& user) {
	std::vector<int>& opList = getChannelOpList();
	typedef std::vector<int>::iterator OpIter;
	int user_sock = user.getUserSock();
	for (OpIter iter = opList.begin();iter != opList.end();iter++) {
		if (*iter == user_sock)
			return ;
	}
	opList.push_back(user.getUserSock());
}

bool Channel::isOperator(User& user) {
	typedef std::vector<int>::iterator OpIter;
	for (OpIter iter = channelOpList.begin();iter != channelOpList.end();iter++) {
		if (*iter == user.getUserSock())
			return true;
	}
	return false;
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

std::vector<int>& Channel::getChannelOpList(void) {
	return channelOpList;
}