#include "User.hpp"

//ToDo: 유저에 대한 operator 권한을 어떻게 할 것인지 정해야함.


User::User(void):
isOperator(false),isChannelJoined(false),joinedChannelIndex(-1),userIndex(-1) {
	userRegister();
	// connectionTime = std::time(nullptr);
}

User::~User() {}

void User::userRegister(void) {
	std::string username;
	std::string nickname;

	std::cout << "Enter Username: ";
	std::cin >> username;
	std::cout << "Enter Nickname: ";
	std::cin >> nickname;

	setUserName(username);
	setNickName(nickname);
}

void User::userInfoErase(User& user) {
	user.setUserName(NULL);
	user.setNickName(NULL);
	user.setIsChannelJoined(false);
	user.setIsOperator(false);
	user.setUserIndex(-1);
	user.setJoinedChannelIndex(-1);
}

void User::joinChannel(std::string toJoinChannel, ChannelContainer& channel_contaier) {
	int	index = 0;
	if (!channel_contaier.getChannelList().empty()) {
		std::vector<Channel*>::iterator channel_iter \
			= channel_contaier.getChannelList().begin();

		for (channel_iter = channel_contaier.getChannelList().begin();\
			channel_iter != channel_contaier.getChannelList().end();channel_iter++) {
			if ((*channel_iter)->getChannelName() == toJoinChannel) {
				setUserIndex(channel_contaier.getChannelList().size());
				(*channel_iter)->userAdd(*this);
				setJoinedChannelIndex(index);
				return ;
			}
			index++;
		}
	}
	std::cout << "no such channel. new channel generate." << std::endl;
	std::string topic;
	std::cout << "channel topic: ";
	std::cin >> topic;
	Channel toJoin(toJoinChannel, topic, 1);
	toJoin.userAdd(*this);
	channel_contaier.createChannel(toJoin);
	setIsChannelJoined(true);
	setJoinedChannelIndex(index);
	setUserIndex(0);
}

void User::exitChannel(ChannelContainer& channel_container) {
	if (getJoinedChannelIndex() < 0) {
		std::cerr << "no joined channel." << std::endl;
		return ;
	}
	std::vector<Channel*>::iterator channel_iter \
		= channel_container.getChannelList().begin() + joinedChannelIndex;
	(*channel_iter)->userDel(*this);
	if (channel_container.getChannelList().empty())
		channel_container.getChannelList().erase\
			(channel_container.getChannelList().begin() + joinedChannelIndex);
	userInfoErase(*this);
}

void User::kickUser(User& toKickUser, ChannelContainer& channel_container) {
	if (!isOperator) {
		std::cerr << "Not permitted." << std::endl;
		return ;
	}
	toKickUser.exitChannel(channel_container);
	std::cout << toKickUser.getNickName() << "is kicked from this channel." << std::endl;
}

void User::inviteUser(User& toInviteUser, ChannelContainer& channel_container) {
	if (!isOperator) {
		std::cerr << "Not permitted." << std::endl;
		return ;
	}
	toInviteUser.joinChannel((*getJoinedChannel(channel_container)).getChannelName(), channel_container);
	std::cout << toInviteUser.getNickName() << "is now added this channel." << std::endl;
}

void User::changeMode(int type, ChannelContainer& channel_container) {
	if (!isOperator) {
		std::cerr << "Not permitted." << std::endl;
		return ;
	}
	(*getJoinedChannel(channel_container)).getModeRef().setType(type);
	std::cout << "now channel mode is " << type << std::endl;
}

void User::changeUserName(std::string toChange) {
	setUserName(toChange);
}

void User::changeNickName(std::string toChange) {
	setNickName(toChange);
}


std::string User::getUserName(void) const {
	return userName;
}

void User::setUserName(std::string const username) {
	userName = username;
}

std::string User::getNickName(void) const {
	return nickName;
}

void User::setNickName(std::string const nickname) {
	nickName = nickname;
}

bool User::getIsOperator(void) const {
	return isOperator;
}

void User::setIsOperator(bool flag) {
	isOperator = flag;
}

bool User::getIsChannelJoined(void) const {
	return isChannelJoined;
}

void User::setIsChannelJoined(bool flag) {
	isChannelJoined = flag;
}

int User::getJoinedChannelIndex(void) const {
	return joinedChannelIndex;
}

void User::setJoinedChannelIndex(int const joined_channel_index) {
	joinedChannelIndex = joined_channel_index;
}

Channel* User::getJoinedChannel(ChannelContainer& channer_container) const {
	std::vector<Channel*>::iterator channel_iter;
	if (joinedChannelIndex >= 0)
		channel_iter = channer_container.getChannelList().begin() + joinedChannelIndex;
	else
		return NULL;
	return *channel_iter;
}

int	User::getUserIndex(void) const {
	return userIndex;
}

void User::setUserIndex(int const index) {
	userIndex = index;
}