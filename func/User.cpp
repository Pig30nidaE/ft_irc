#include "User.hpp"

//ToDo: 유저에 대한 operator 권한을 어떻게 할 것인지 정해야함.


User::User(void):
isOperator(false),userIndex(-1) {
	userRegister();
	// connectionTime = std::time(nullptr);
}

User::~User() {}

void User::userRegister(void) {
	std::string username;
	std::string nickname;

	std::cout << "Enter Username: ";
	while (true) {
		std::getline(std::cin, username);

	}
	std::cout << "Enter Nickname: ";
	std::getline(std::cin, nickname);

	setUserName(username);
	setNickName(nickname);
}

void User::userInfoErase(User& user) {
	user.setUserName("");
	user.setNickName("");
	//user.setIsChannelJoined(false);
	user.setIsOperator(false);
	user.setUserIndex(-1);
	//user.setJoinedChannelIndex(-1);
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
				//setJoinedChannelIndex(index);
				return ;
			}
			index++;
		}
	}
	std::cout << "no such channel. new channel generate." << std::endl;
	std::string topic;
	std::cout << "channel topic: ";
	std::getline(std::cin, topic);
	Channel *toJoin = new Channel(toJoinChannel, topic, 1);
	toJoin->userAdd(*this);
	channel_contaier.createChannel(*toJoin);
	this->joinedChannelList.push_back(toJoin);
	//setIsChannelJoined(true);
	//setJoinedChannelIndex(index);
	setUserIndex(0);
}

void User::exitChannel(std::string toExitChannel, ChannelContainer& channel_container) {
    Channel *exitCh = getSpecificChannelJoined(toExitChannel, channel_container);
    std::vector<Channel*>& JoinedChannels = getJoinedChannelList(channel_container);

    exitCh->userDel(*this);

    std::vector<Channel*>::iterator iter = std::find(JoinedChannels.begin(), JoinedChannels.end(), exitCh);
    if (iter != JoinedChannels.end()) {
        JoinedChannels.erase(iter);
    }
}


void User::kickUser(User& toKickUser, Channel& fromChannel, ChannelContainer& channel_container) {
	if (!isOperator) {
		std::cerr << "Not permitted." << std::endl;
		return ;
	}
	if (toKickUser.getUserName() == (*this).getUserName()) {
		std::cerr << "Can't kick yourself." << std::endl;
		return ;
	}
	toKickUser.exitChannel(fromChannel.getChannelName(), channel_container);
	std::cout << toKickUser.getNickName() << "is kicked from this channel." << std::endl;
}

void User::inviteUser(User& toInviteUser, ChannelContainer& channel_container) {
	if (!isOperator) {
		std::cerr << "Not permitted." << std::endl;
		return ;
	}
	if (toInviteUser.getUserName() == (*this).getUserName()) {
		std::cerr << "Can't invite yourself." << std::endl;
		return ;
	}
	//toInviteUser.joinChannel((getJoinedChannel(channel_container)).getChannelName(), channel_container);
	std::cout << toInviteUser.getNickName() << "is now added this channel." << std::endl;
}

void User::changeMode(int type, ChannelContainer& channel_container) {
	if (!isOperator) {
		std::cerr << "Not permitted." << std::endl;
		return ;
	}
	//getJoinedChannel(channel_container).getModeRef().setType(type);
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

//std::string User::getNickName(void) const {
//	return nickName;
//}

//void User::setNickName(std::string const nickname) {
//	nickName = nickname;
//}

bool User::getIsOperator(void) const {
	return isOperator;
}

void User::setIsOperator(bool flag) {
	isOperator = flag;
}

//bool User::getIsChannelJoined(void) const {
//	return isChannelJoined;
//}

//void User::setIsChannelJoined(bool flag) {
//	isChannelJoined = flag;
//}

//int User::getJoinedChannelIndex(void) const {
//	return joinedChannelIndex;
//}

//void User::setJoinedChannelIndex(int const joined_channel_index) {
//	joinedChannelIndex = joined_channel_index;
//}

std::vector<Channel*>& User::getJoinedChannelList(ChannelContainer& channel_container) {
	//if (getJoinedChannelIndex() < 0)
	//	throw NoJoinedChannelException();
	return joinedChannelList;
}

Channel* User::getSpecificChannelJoined(std::string toGet, ChannelContainer& channel_container) {
	std::vector<Channel*>::iterator ch_iter = getJoinedChannelList(channel_container).begin();

	for (ch_iter = getJoinedChannelList(channel_container).begin();\
		ch_iter != getJoinedChannelList(channel_container).end();ch_iter++) {
			if ((*ch_iter)->getChannelName() == toGet)
				return *ch_iter;
		}
	return nullptr;
}

int	User::getUserIndex(void) const {
	return userIndex;
}

void User::setUserIndex(int const index) {
	userIndex = index;
}