#include "User.hpp"

//ToDo: 유저에 대한 operator 권한을 어떻게 할 것인지 정해야함.


User::User(ChannelContainer &channel_container):
isOperator(false),userIndex(-1) {
	userRegister(channel_container);
	// connectionTime = std::time(nullptr);
}

User::~User() {}

void User::userRegister(ChannelContainer &channel_container) {
	std::string enteredUserName;
	typedef std::vector<User*>::iterator UserIterator;
	UserIterator user_iter;
	std::vector<User*> user_list = channel_container.getUserList();

	while (true) {
		bool flag = true;
		std::cout << "Enter username: ";
		std::getline(std::cin, enteredUserName);
		for (user_iter = user_list.begin();user_iter != user_list.end();user_iter++) {
			if ((*user_iter)->getUserName() == enteredUserName) {
				std::cout << "Already exist. Please enter another user name." << std::endl;
				flag = false;
				break ;
			}
		}
		if (flag)
			break ;
	}
	setUserName(enteredUserName);
}

void User::nickRegister(Channel& channel, ChannelContainer& channel_container) {
	typedef std::vector<User*>::iterator UserIterator;
	std::vector<User*> user_list = channel.getUserList();
	std::string to_set_nick;
	while (true) {
		bool flag = true;
		std::cout << "Enter nickname: ";
		std::getline(std::cin, to_set_nick);
		for (UserIterator user_iter = user_list.begin();user_iter != user_list.end();user_iter++) {
			if ((*user_iter)->getSpcfNickName(channel.getChannelName()) == to_set_nick) {
				std::cout << "Already exist." << std::endl;
				flag = false;
				break ;
			}
		}
		if (flag)
			break ;
	}
	setNickName(channel.getChannelName(), to_set_nick);
}

void User::userInfoErase(User& user) {
	user.setUserName("");
	//user.setIsChannelJoined(false);
	user.setIsOperator(false);
	user.setUserIndex(-1);
	//user.setJoinedChannelIndex(-1);
}

void User::joinChannel(std::string toJoinChannel, ChannelContainer& channel_contaier, bool isInvited) {
	int	index = 0;
	if (!channel_contaier.getChannelList().empty()) {
		std::vector<Channel*>::iterator channel_iter \
			= channel_contaier.getChannelList().begin();

		for (channel_iter = channel_contaier.getChannelList().begin();\
			channel_iter != channel_contaier.getChannelList().end();channel_iter++) {
			if ((*channel_iter)->getChannelName() == toJoinChannel) {
				Mode& channel_mode = (*channel_iter)->getModeRef();
				//int channel_mode = (*channel_iter)->getModeRef().getType();
				if (channel_mode.checkType(MODE_CHANNEL_KEY)) {
					std::string enterPwd;
					std::cout << "PASSWORD: ";
					std::getline(std::cin, enterPwd);
					if (enterPwd != (*channel_iter)->getPassword()) {
						std::cout << "wrong password." << std::endl;
						return ;
					}
				}
				if (channel_mode.checkType(MODE_INVITE_ONLY)) {
					if (!isInvited) {
						std::cout << "This channel is invite only." << std::endl;
						return ;
					}
				}
				setUserIndex(channel_contaier.getChannelList().size());
				nickRegister(*(*channel_iter), channel_contaier);
				(*channel_iter)->userAdd(*this);
				getJoinedChannelList(channel_contaier).push_back(*channel_iter);
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
	nickRegister(*toJoin, channel_contaier);
	setIsOperator(true);
	toJoin->userAdd(*this);
	channel_contaier.createChannel(*toJoin);
	getJoinedChannelList(channel_contaier).push_back(toJoin);
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
	if (exitCh->getUserList().empty())
		channel_container.deleteChannel(*exitCh);
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
	typedef std::vector<User*>::iterator UserIterator;
	std::vector <User*> toChUsrList = fromChannel.getUserList();
	for (UserIterator user_iter = toChUsrList.begin();user_iter != toChUsrList.end();user_iter++) {
		if ((*user_iter)->getUserName() == toKickUser.getUserName()) {
			toKickUser.exitChannel(fromChannel.getChannelName(), channel_container);
			std::cout << toKickUser.getSpcfNickName(fromChannel.getChannelName()) << " is kicked from this channel." << std::endl;
			return ;
		}
	}
	std::cerr << "No such user." << std::endl;
}

void User::inviteUser(User& toInviteUser, Channel& toChannel, ChannelContainer& channel_container) {
	if (!isOperator) {
		std::cerr << "Not permitted." << std::endl;
		return ;
	}
	if (toInviteUser.getUserName() == (*this).getUserName()) {
		std::cerr << "Can't invite yourself." << std::endl;
		return ;
	}
	typedef std::vector<User*>::iterator UserIterator;
	std::vector <User*> toChUsrList = toChannel.getUserList();
	for (UserIterator user_iter = toChUsrList.begin();user_iter != toChUsrList.end();user_iter++) {
		if ((*user_iter)->getUserName() == toInviteUser.getUserName()) {
			std::cerr << "Already exist in the channel." << std::endl;
			return ;
		}
	}
	toInviteUser.joinChannel(toChannel.getChannelName(), channel_container, true);
}

void User::changeMode(ChannelContainer& channel_container, std::string channel, int type, ...) {
	if (!isOperator) {
		std::cerr << "Not permitted." << std::endl;
		return ;
	}
	Channel* joinedChannel = getSpecificChannelJoined(channel, channel_container);
	if (!joinedChannel) {
		std::cerr << "no such channel." << std::endl;
		return ;
	}
	Mode& channelMode = joinedChannel->getModeRef();
	if (MODE_CHECK_TYPE(type, MODE_INVITE_ONLY)) {
		channelMode.turnOnType(MODE_INVITE_ONLY);
	}
	if (MODE_CHECK_TYPE(type, MODE_TOPIC_LIMIT)) {
		channelMode.turnOnType(MODE_TOPIC_LIMIT);
	}
	if (MODE_CHECK_TYPE(type, MODE_CHANNEL_KEY)) {
		channelMode.turnOnType(MODE_CHANNEL_KEY);
		va_list ap;
		va_start(ap, type);
		std::string passwd(va_arg(ap, char*));
		joinedChannel->setPassword(passwd);
	}
	if (MODE_CHECK_TYPE(type, MODE_OPERATOR_ONLY)) {
		va_list ap;
		va_start(ap, type);
		std::string targetNick(va_arg(ap, char *));
		std::vector<User*> usrList = joinedChannel->getUserList();
		typedef std::vector<User*>::iterator usrIterator;
		for (usrIterator user_iter = usrList.begin();user_iter != usrList.end();user_iter++) {
			if ((*user_iter)->getSpcfNickName(joinedChannel->getChannelName()) == targetNick) {
				channelMode.turnOnType(MODE_OPERATOR_ONLY);
				(*user_iter)->setIsOperator(true);
				return ;
			}
		}
		std::cout << "no such user." << std::endl;
	}
}

void User::topicCommand(ChannelContainer& channel_container, std::string channel, ...) {
	Channel* joinedChannel = getSpecificChannelJoined(channel, channel_container);
	if (!joinedChannel) {
		std::cerr << "no such channel." << std::endl;
		return ;
	}
	Mode& mode = joinedChannel->getModeRef();
	if (mode.checkType(MODE_TOPIC_LIMIT)) {
		if (getIsOperator() == false) {
			std::cerr << "only operator can change topic." << std::endl;
			return ;
		}
	}
	va_list ap;
	va_start(ap, channel);
	std::string newTopic(va_arg(ap, char*));
	if (!newTopic.length()) {
		std::cout << joinedChannel->getTopicRef().getTopic() << std::endl;
	}
	else {
		joinedChannel->getTopicRef().setTopic(newTopic);
	}
}

void User::changeUserName(std::string toChange) {
	setUserName(toChange);
}

//void User::changeNickName(std::string toChangeCh, std::string toChangeNick) {
//	setNickName(toChangeCh, toChangeNick);
//}


std::string User::getUserName(void) const {
	return userName;
}

void User::setUserName(std::string const username) {
	userName = username;
}

std::string User::getSpcfNickName(std::string chName) {
	typedef std::map<std::string, std::string>::iterator NickIterator;
	for (NickIterator nick_iter = nickName.begin();nick_iter != nickName.end();nick_iter++) {
		if ((*nick_iter).first == chName)
			return (*nick_iter).second;
	}
	return "";
}

std::map<std::string, std::string> User::getNickName(void) {
	return nickName;
}

void User::setNickName(std::string chName, std::string toChangeNick) {
	std::map<std::string, std::string>::iterator nick_iter;
	nick_iter = nickName.find(chName);
	if (nick_iter != nickName.end()) {
		nick_iter->second = toChangeNick;
	}
	else
		nickName.insert(std::make_pair(chName, toChangeNick));
}

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