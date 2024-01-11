#include "User.hpp"
#include "../source/Command.hpp"


User::User(void):
buf(""),userSock(-1),isPass(0),userName(""),nickName(""),hostName(""),client(""),userIndex(-1) {
	std::cout << "[USER] constructer user: " << userSock << "\n";
}

User::~User() {std::cout << "[USER] destroyed user: " << userSock << "\n";}


void User::userInfoErase(User& user) {
	user.setUserName("");
	//user.setIsChannelJoined(false);
	//user.setIsOperator(false);
	user.setUserIndex(-1);
	//user.setJoinedChannelIndex(-1);
}

int User::joinChannel(std::string toJoinChannel, std::string enterPwd, ChannelContainer& channel_contaier) {
	int	index = 0;
	if (!channel_contaier.getChannelList().empty()) {
		std::vector<Channel*>::iterator channel_iter \
			= channel_contaier.getChannelList().begin();

		for (channel_iter = channel_contaier.getChannelList().begin();\
			channel_iter != channel_contaier.getChannelList().end();channel_iter++) {
			if ((*channel_iter)->getChannelName() == toJoinChannel) {
				int channel_capacity = (*channel_iter)->getCapacity();
				if (channel_capacity) {
					if ((*channel_iter)->getUserList().size() == channel_capacity) {
						return ERR_CHANNELISFULL_NUM;
					}
				}
				Mode& channel_mode = (*channel_iter)->getModeRef();
				if (channel_mode.checkType(MODE_CHANNEL_KEY)) {
					if (enterPwd.length()) {
						if (enterPwd != (*channel_iter)->getPassword()) {
							return ERR_BADCHANNELKEY_NUM; //BadChannelKey
						}
					}
					else { // key가 필요한데 제공되지 않은것도 BADCHANNELKEY
						return ERR_BADCHANNELKEY_NUM; // BadChannelKey
					}
				}
				if (channel_mode.checkType(MODE_INVITE_ONLY)) {
					typedef std::vector<int>::iterator INVITED;
					INVITED iv_iter_begin = (*channel_iter)->getInvitedUserList().begin();
					INVITED iv_iter_end = (*channel_iter)->getInvitedUserList().end();
					if (std::find(iv_iter_begin, iv_iter_end, getUserSock()) == iv_iter_end) {
						return ERR_INVITEONLYCHAN_NUM; // InviteOnlyChan
					}
				}
				setUserIndex(channel_contaier.getChannelList().size());
				(*channel_iter)->userAdd(*this);
				getJoinedChannelList(channel_contaier).push_back(*channel_iter);
				return JOINCHANNEL;
			}
			index++;
		}
	}
	//std::cout << "no such channel. new channel generate." << std::endl;
	Channel *toJoin = new Channel(toJoinChannel);
	toJoin->userAdd(*this);
	toJoin->operatorAdd(*this);
	//setIsOperator(true);
	channel_contaier.createChannel(*toJoin);
	getJoinedChannelList(channel_contaier).push_back(toJoin);
	setUserIndex(0);
	return NEWCHANNEL;
}

int User::exitChannel(std::string toExitChannel, ChannelContainer& channel_container) {
    Channel *exitCh = getSpecificChannelJoined(toExitChannel, channel_container);
	if (!exitCh) //내가 가입한 채널 중에 없음
		return ERR_NOTONCHANNEL_NUM;
    std::vector<Channel*>& JoinedChannels = getJoinedChannelList(channel_container);

	//std::cout << "CHECKER1" << std::endl;
    exitCh->userDel(*this); //

    std::vector<Channel*>::iterator iter = std::find(JoinedChannels.begin(), JoinedChannels.end(), exitCh);
    if (iter != JoinedChannels.end()) {
		std::vector<int>::iterator sock_iter = std::find(exitCh->getInvitedUserList().begin(),\
			exitCh->getInvitedUserList().end(), getUserSock());
		if (sock_iter != exitCh->getInvitedUserList().end())
			exitCh->getInvitedUserList().erase(sock_iter);
        JoinedChannels.erase(iter);
    }
	if (exitCh->getUserList().empty())
		channel_container.deleteChannel(*exitCh);
	else {
		std::vector<int>& OpList = exitCh->getChannelOpList();
		if (OpList.empty()) {
			OpList.push_back(exitCh->getUserList().at(0)->getUserSock());
		}
	}
	return 0;
}


void User::kickUser(std::string toKickUser, Channel& fromChannel, ChannelContainer& channel_container) {
	//std::cout << toKickUser << std::endl;
	//std::cout << this->getUserName() << std::endl;
	//if (!fromChannel.isOperator(*this)) {
	//	std::cerr << "Not permitted." << std::endl;
	//	return ;
	//}
	if (toKickUser == getNickName()) {
		std::cerr << "Can't kick yourself." << std::endl;
		return ;
	}
	//if (fromChannel == NULL) {
	//	return ; // fromChannel없을때
	//}
	//typedef std::vector<User*>::iterator UserIterator;
	//std::vector <User*> toChUsrList = fromChannel.getUserList();
	//for (UserIterator user_iter = toChUsrList.begin();user_iter != toChUsrList.end();user_iter++) {
	//	if ((*user_iter)->getNickName() == toKickUser) {
	//		(*user_iter)->exitChannel(fromChannel.getChannelName(), channel_container);
	//		return ;
	//	}
	//}
	fromChannel.getSpecficUser(toKickUser)->exitChannel(fromChannel.getChannelName(), channel_container);
	//std::cerr << "No such user." << std::endl;
}

void User::inviteUser(User& toInviteUser, Channel& toChannel, ChannelContainer& channel_container) {
	if (!toChannel.isOperator(*this)) {
		std::cerr << "Not permitted." << std::endl;
		return ;
	}
	if (toInviteUser.getNickName() == (*this).getNickName()) {
		std::cerr << "Can't invite yourself." << std::endl;
		return ;
	}
	typedef std::vector<int>::iterator InvitedIterator;
	std::vector <int> invUsrLst = toChannel.getInvitedUserList();
	for (InvitedIterator inv_iter = invUsrLst.begin();inv_iter != invUsrLst.end();inv_iter++) {
		if (*inv_iter == toInviteUser.getUserSock()) {
			std::cerr << "Already invited." << std::endl;
			return ;
		}
	}
	toChannel.getInvitedUserList().push_back(toInviteUser.getUserSock());
	//std::cout << toInviteUser.getNickName() << " is invited." << std::endl;
	//if (toChannel.getModeRef().checkType(MODE_CHANNEL_KEY))
	//	toInviteUser.joinChannel(toChannel.getChannelName(), enterPwd, channel_container, true);
	//else
	//	toInviteUser.joinChannel(toChannel.getChannelName(), "",channel_container, true);
}

// void User::changeMode(ChannelContainer& channel_container, std::string channel, bool onoff_flag, int type, ...) {
int User::changeMode(ChannelContainer& channel_container, Channel &joinedChannel, bool onoff_flag, int type, std::string arg) {
	if (MODE_CHECK_TYPE(type, MODE_OPERATOR_GRANTED) || MODE_CHECK_TYPE(type, MODE_CHANNEL_KEY) || MODE_CHECK_TYPE(type, MODE_CAPACITY_LIMIT)) {
		if (arg == "") {
			if (MODE_CHECK_TYPE(type, MODE_OPERATOR_GRANTED))
				throw Command::ErrInvalidModeParam(this->getNickName(), joinedChannel.getChannelName(), "o", "You must specify a parameter for the op mode. Syntax: <nick>.");
			else if (MODE_CHECK_TYPE(type, MODE_CHANNEL_KEY))
				throw Command::ErrInvalidModeParam(this->getNickName(), joinedChannel.getChannelName(), "k", "You must specify a parameter for the key mode. Syntax: <key>.");
			else if (MODE_CHECK_TYPE(type, MODE_CAPACITY_LIMIT) && onoff_flag) {
				throw Command::ErrInvalidModeParam(this->getNickName(), joinedChannel.getChannelName(), "l", "You must specify a parameter for the limit mode. Syntax: <limit>.");
			}
		}
	}
	if (!(joinedChannel.isOperator(*this))) {
		throw Command::ErrChanOPrivsNeeded(joinedChannel.getChannelName());
	}
	Mode& channelMode = joinedChannel.getModeRef();
	if (onoff_flag) { // MODE ON
		if (MODE_CHECK_TYPE(type, MODE_INVITE_ONLY))
			channelMode.turnOnType(MODE_INVITE_ONLY);
		if (MODE_CHECK_TYPE(type, MODE_TOPIC_LIMIT))
			channelMode.turnOnType(MODE_TOPIC_LIMIT);
		if (MODE_CHECK_TYPE(type, MODE_CHANNEL_KEY)) { // k
			if (channelMode.checkType(MODE_CHANNEL_KEY))
				return 1;
			channelMode.turnOnType(MODE_CHANNEL_KEY);
			if (arg == "")
				throw Command::ErrInvalidModeParam(this->getNickName(), joinedChannel.getChannelName(), "k", "You must specify a parameter for the key mode. Syntax: <key>.");
			joinedChannel.setPassword(arg);
		}
		if (MODE_CHECK_TYPE(type, MODE_OPERATOR_GRANTED)) { // o
			if (arg == "")
				throw Command::ErrInvalidModeParam(this->getNickName(), joinedChannel.getChannelName(), "o", "You must specify a parameter for the op mode. Syntax: <nick>.");
			std::vector<User*> usrList = joinedChannel.getUserList();
			typedef std::vector<User*>::iterator usrIterator;
			for (usrIterator user_iter = usrList.begin();user_iter != usrList.end();user_iter++) {
				if ((*user_iter)->getNickName() == arg) {
					if (joinedChannel.isClientOpList((*user_iter)->getUserSock()))
						return 1;
					channelMode.turnOnType(MODE_OPERATOR_GRANTED);
					(*user_iter)->getSpecificChannelJoined(joinedChannel.getChannelName(), channel_container)->getChannelOpList().\
						push_back((*user_iter)->getUserSock());
					return 0;
				}
			}
			throw Command::ErrNoSuchNick(this->getNickName(), arg); // noSuchNick
		}
		if (MODE_CHECK_TYPE(type, MODE_CAPACITY_LIMIT)) { // l
			if (arg == "")
				throw Command::ErrInvalidModeParam(this->getNickName(), joinedChannel.getChannelName(), "l", "You must specify a parameter for the limit mode. Syntax: <limit>.");
			int size = stringToInt(arg);
			if (size < 0) // 마니어스이거나 변환 에러가 났을 때
				throw Command::ErrInvalidModeParam(this->getNickName(), joinedChannel.getChannelName(), "l", "Invalid limit mode parameter. Syntax: <limit>.");
			if (joinedChannel.getCapacity() == size) {
				return 1;
			}
			if (size == 0)
				size = 1;
			joinedChannel.setCapacity(size);
		}
	}
	else { // MODE OFF
		if (MODE_CHECK_TYPE(type, MODE_INVITE_ONLY))
			channelMode.turnOffType(MODE_INVITE_ONLY);
		if (MODE_CHECK_TYPE(type, MODE_TOPIC_LIMIT))
			channelMode.turnOffType(MODE_TOPIC_LIMIT);
		if (MODE_CHECK_TYPE(type, MODE_CHANNEL_KEY)) {
			if (!channelMode.checkType(MODE_CHANNEL_KEY))
				return 1;
			if (arg == "")
				throw Command::ErrInvalidModeParam(this->getNickName(), joinedChannel.getChannelName(), "k", "You must specify a parameter for the key mode. Syntax: <key>.");
			std::cout << "request key: " << arg << "\n";
			if (arg == joinedChannel.getPassword()) {
				joinedChannel.setPassword("");
				channelMode.turnOffType(MODE_CHANNEL_KEY);
			}
			else {
				throw Command::ErrKeyMissMatch(this->getNickName(), joinedChannel.getChannelName());
			}
		}
		if (MODE_CHECK_TYPE(type, MODE_OPERATOR_GRANTED)) {
			if (arg == "")
				throw Command::ErrInvalidModeParam(this->getNickName(), joinedChannel.getChannelName(), "o", "You must specify a parameter for the op mode. Syntax: <nick>.");
			std::vector<User*> usrList = joinedChannel.getUserList();
			typedef std::vector<User*>::iterator usrIterator;
			for (usrIterator user_iter = usrList.begin();user_iter != usrList.end();user_iter++) {
				if ((*user_iter)->getNickName() == arg) {
					if (!joinedChannel.isClientOpList((*user_iter)->getUserSock()))
						return 1;
					channelMode.turnOffType(MODE_OPERATOR_GRANTED);
					std::vector<int>& OpList = (*user_iter)->getSpecificChannelJoined(joinedChannel.getChannelName(), channel_container)->getChannelOpList();
					typedef std::vector<int>::iterator OpIter;
					for (OpIter iter = OpList.begin();iter != OpList.end();iter++) {
						if (*iter == (*user_iter)->getUserSock()) {
							OpList.erase(iter);
							return 0;
						}
					}
					//(*user_iter)->setIsOperator(false);
					//(*user_iter)->getSpecificChannelJoined(channel, channel_container)->getChannelOpList().erase(user_iter);
					return 0;
				}
			}
			throw Command::ErrNoSuchNick(this->getNickName(), arg); // noSuchNick
		}
		if (MODE_CHECK_TYPE(type, MODE_CAPACITY_LIMIT)) {
			if (joinedChannel.getCapacity() == 0)
				return 1;
			joinedChannel.setCapacity(0);
		}
	}
	// Channel* joinedChannel = getSpecificChannelJoined(channel, channel_container);
	// if (!joinedChannel) {
	// 	std::cerr << "no such channel." << std::endl;
	// 	return ;
	// }
	// if (!(joinedChannel.isOperator(*this))) {
	// 	std::cerr << "Not permitted." << std::endl;
	// 	return ;
	// }
	// Mode& channelMode = joinedChannel->getModeRef();
	// //std::cout << "type: " << type << std::endl;
	// if (onoff_flag) { // MODE ON
	// 	if (MODE_CHECK_TYPE(type, MODE_INVITE_ONLY))
	// 		channelMode.turnOnType(MODE_INVITE_ONLY);
	// 	if (MODE_CHECK_TYPE(type, MODE_TOPIC_LIMIT))
	// 		channelMode.turnOnType(MODE_TOPIC_LIMIT);
	// 	if (MODE_CHECK_TYPE(type, MODE_CHANNEL_KEY)) {
	// 		channelMode.turnOnType(MODE_CHANNEL_KEY);
	// 		va_list ap;
	// 		va_start(ap, type);
	// 		char *passwd_ap = va_arg(ap, char*);
	// 		if (!passwd_ap)
	// 			return ;
	// 		std::string passwd(passwd_ap);
	// 		//std::cout << passwd << std::endl;
	// 		joinedChannel->setPassword(passwd);
	// 	}
	// 	if (MODE_CHECK_TYPE(type, MODE_OPERATOR_GRANTED)) {
	// 		va_list ap;
	// 		va_start(ap, type);
	// 		char *target_ap = va_arg(ap, char*);
	// 		if (!target_ap)
	// 			return ;
	// 		std::string targetNick(target_ap);
	// 		//std::string targetNick(va_arg(ap, char *));
	// 		std::vector<User*> usrList = joinedChannel->getUserList();
	// 		typedef std::vector<User*>::iterator usrIterator;
	// 		for (usrIterator user_iter = usrList.begin();user_iter != usrList.end();user_iter++) {
	// 			if ((*user_iter)->getNickName() == targetNick) {
	// 				channelMode.turnOnType(MODE_OPERATOR_GRANTED);
	// 				(*user_iter)->getSpecificChannelJoined(channel, channel_container)->getChannelOpList().\
	// 					push_back((*user_iter)->getUserSock());
	// 				return ;
	// 			}
	// 		}
	// 		std::cout << "no such user." << std::endl;
	// 	}
	// 	if (MODE_CHECK_TYPE(type, MODE_CAPACITY_LIMIT)) {
	// 		va_list ap;
	// 		va_start(ap, type);
	// 		int size = va_arg(ap, int);
	// 		if (!size)
	// 			return ;
	// 		joinedChannel->setCapacity(size);
	// 	}
	// }
	// else { // MODE OFF
	// 	if (MODE_CHECK_TYPE(type, MODE_INVITE_ONLY))
	// 		channelMode.turnOffType(MODE_INVITE_ONLY);
	// 	if (MODE_CHECK_TYPE(type, MODE_TOPIC_LIMIT))
	// 		channelMode.turnOffType(MODE_TOPIC_LIMIT);
	// 	if (MODE_CHECK_TYPE(type, MODE_CHANNEL_KEY)) {
	// 		va_list ap;
	// 		va_start(ap, type);
	// 		char* passwd_ap = va_arg(ap, char*);
	// 		if (!passwd_ap)
	// 			return ;
	// 		std::string passwd(passwd_ap);
	// 		if (passwd == joinedChannel->getPassword()) {
	// 			joinedChannel->setPassword("");
	// 			channelMode.turnOffType(MODE_CHANNEL_KEY);
	// 		}
	// 		else {
	// 			std::cerr << "password incorrect." << std::endl;
	// 		}
	// 	}
	// 	if (MODE_CHECK_TYPE(type, MODE_OPERATOR_GRANTED)) {
	// 		va_list ap;
	// 		va_start(ap, type);
	// 		std::string targetNick(va_arg(ap, char *));
	// 		std::vector<User*> usrList = joinedChannel->getUserList();
	// 		typedef std::vector<User*>::iterator usrIterator;
	// 		for (usrIterator user_iter = usrList.begin();user_iter != usrList.end();user_iter++) {
	// 			if ((*user_iter)->getNickName() == targetNick) {
	// 				channelMode.turnOffType(MODE_OPERATOR_GRANTED);
	// 				std::vector<int>& OpList = (*user_iter)->getSpecificChannelJoined(channel, channel_container)->getChannelOpList();
	// 				typedef std::vector<int>::iterator OpIter;
	// 				for (OpIter iter = OpList.begin();iter != OpList.end();iter++) {
	// 					if (*iter == (*user_iter)->getUserSock()) {
	// 						OpList.erase(iter);
	// 						return ;
	// 					}
	// 				}
	// 				//(*user_iter)->setIsOperator(false);
	// 				//(*user_iter)->getSpecificChannelJoined(channel, channel_container)->getChannelOpList().erase(user_iter);
	// 				return ;
	// 			}
	// 		}
	// 		std::cout << "no such user." << std::endl;
	// 	}
	// 	//if (MODE_CHECK_TYPE(type, MODE_CAPACITY_LIMIT)) {
	// 	//	va_list ap;
	// 	//	va_start(ap, type);
	// 	//	int size = va_arg(ap, int);
	// 	//	if (!size)
	// 	//		return ;
	// 	//	joinedChannel->setCapacity(size);
	// 	//}
	// 	if (MODE_CHECK_TYPE(type, MODE_CAPACITY_LIMIT))
	// 		joinedChannel->setCapacity(0);
	// }
	return 0;
}

// void User::topicCommand(ChannelContainer& channel_container, std::string channel, ...) {
// 	Channel* joinedChannel = getSpecificChannelJoined(channel, channel_container);
// 	if (!joinedChannel) {
// 		std::cerr << "no such channel." << std::endl;
// 		return ;
// 	}
// 	Mode& mode = joinedChannel->getModeRef();
// 	if (mode.checkType(MODE_TOPIC_LIMIT)) {
// 		if (getIsOperator() == false) {
// 			std::cerr << "only operator can change topic." << std::endl;
// 			return ;
// 		}
// 	}
// 	va_list ap;
// 	va_start(ap, channel);
// 	char *topic_ap = va_arg(ap, char*);
// 	std::string newTopic(topic_ap);
// 	if (!newTopic.length()) {
// 		std::cout << joinedChannel->getTopicRef().getTopic() << std::endl;
// 	}
// 	else {
// 		joinedChannel->getTopicRef().setTopic(newTopic);
// 	}
// }

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

std::string User::getHostName(void) const {
	return hostName;
}

void User::setHostName(std::string const hostname) {
	hostName = hostname;
}

//std::string User::getNickName(std::string chName) {
//	typedef std::map<std::string, std::string>::iterator NickIterator;
//	for (NickIterator nick_iter = nickName.begin();nick_iter != nickName.end();nick_iter++) {
//		if ((*nick_iter).first == chName)
//			return (*nick_iter).second;
//	}
//	return "";
//}

std::string User::getNickName(void) const {
	return nickName;
}

void User::setNickName(std::string toChangeNick){//, ChannelContainer& channel_container) {
	//typedef std::vector<User*>::iterator UserIterator;
	//std::vector<User*> &user_list = channel_container.getUserList();
	//for (UserIterator user_iter = user_list.begin();\
	//	user_iter != user_list.end();user_iter++) {
	//	if ((*user_iter)->getNickName() == toChangeNick) {
	//		std::cerr << "already exist." << std::endl;
	//		return ;
	//	}
	//}
	nickName = toChangeNick;
}

std::string User::getRealName(void) const {
	return realName;
}

void User::setRealName(std::string realname) {
	realName = realname;
}

std::string User::getClient(void) const {
	return client;
}

void User::setClient(const std::string &nick, const std::string &user, const std::string &host) {
	client = nick + '!' + user + '@' + host;
}

//bool User::getIsOperator(void) const {
//	return isOperator;
//}

//void User::setIsOperator(bool flag) {
//	isOperator = flag;
//}

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
	return NULL;
}

int	User::checkChannelInList(const std::string &channelName, ChannelContainer& cc) {
	std::vector<Channel *> &channelList = cc.getChannelList();
	for (std::vector<Channel *>::iterator iter = channelList.begin(); iter != channelList.end(); iter++) {
		if ((*iter)->getChannelName() == channelName) {
			return 0;
		}
	}
	return ERR_NOSUCHCHANNEL_NUM;
}

int	User::getUserIndex(void) const {
	return userIndex;
}

void User::setUserIndex(int const index) {
	userIndex = index;
}

//------------------------------------------
void User::setUserSock(int sock) {
	userSock = sock;
}

int User::getUserSock(void) const {
	return userSock;
}

void User::setUserIsPass(int flag) {
	isPass = flag;
}

int User::getUserIsPass(void) const {
	return isPass;
}

void User::setBuf(const std::string &buf) {
	this->buf = buf;
}

std::string User::getBuf(void) const {
	return buf;
}

void User::setSendBuf(const std::string &sendBuf) {
	(this->sendBuf).push_back(sendBuf);
}

std::vector<std::string> User::getSendBuf(void) const {
	return sendBuf;
}

void User::initSendBuf(void) {
	sendBuf.clear();
}

int User::stringToInt(std::string str) {
	int num;
	std::istringstream iss(str);

	if (!(iss >> num)) {
		return -1;
	}
    return num;
}