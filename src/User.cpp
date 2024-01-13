#include "../include/User.hpp"
#include "../include/Command.hpp"


User::User(void):
buf(""),userSock(-1),isPass(0),userName(""),nickName(""),hostName(""), client("") {}

User::~User() {}

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
					if ((*channel_iter)->getUserList().size() == (unsigned long)channel_capacity) {
						return ERR_CHANNELISFULL_NUM;
					}
				}
				Mode& channel_mode = (*channel_iter)->getModeRef();
				if (channel_mode.checkType(MODE_CHANNEL_KEY)) {
					if (enterPwd.length()) {
						if (enterPwd != (*channel_iter)->getPassword()) {
							return ERR_BADCHANNELKEY_NUM;
						}
					}
					else {
						return ERR_BADCHANNELKEY_NUM;
					}
				}
				if (channel_mode.checkType(MODE_INVITE_ONLY)) {
					typedef std::vector<int>::iterator INVITED;
					INVITED iv_iter_begin = (*channel_iter)->getInvitedUserList().begin();
					INVITED iv_iter_end = (*channel_iter)->getInvitedUserList().end();
					if (std::find(iv_iter_begin, iv_iter_end, getUserSock()) == iv_iter_end) {
						return ERR_INVITEONLYCHAN_NUM;
					}
				}
				if ((*channel_iter)->isOnTheChan(*this)) {
					return ERR_ALREADYONCHAN;
				}
				(*channel_iter)->userAdd(*this);
				getJoinedChannelList().push_back(*channel_iter);
				return JOINCHANNEL;
			}
			index++;
		}
	}
	Channel *toJoin = new Channel(toJoinChannel);
	toJoin->userAdd(*this);
	toJoin->operatorAdd(*this);
	channel_contaier.createChannel(*toJoin);
	getJoinedChannelList().push_back(toJoin);
	return NEWCHANNEL;
}

int User::exitChannel(std::string toExitChannel, ChannelContainer& channel_container) {
    Channel *exitCh = getSpecificChannelJoined(toExitChannel);

	if (!exitCh)
		return ERR_NOTONCHANNEL_NUM;
    std::vector<Channel*>& JoinedChannels = getJoinedChannelList();
    exitCh->userDel(*this);

    std::vector<Channel*>::iterator iter = std::find(JoinedChannels.begin(), JoinedChannels.end(), exitCh);
    if (iter != JoinedChannels.end()) {
		std::vector<int>::iterator sock_iter = std::find(exitCh->getInvitedUserList().begin(),\
			exitCh->getInvitedUserList().end(), getUserSock());
		if (sock_iter != exitCh->getInvitedUserList().end())
			exitCh->getInvitedUserList().erase(sock_iter);
        JoinedChannels.erase(iter);
    }
	if (exitCh->getUserList().empty()) {
		channel_container.deleteChannel(*exitCh);
	}
	else {
		std::vector<int>& OpList = exitCh->getChannelOpList();
		if (OpList.empty()) {
			OpList.push_back(exitCh->getUserList().at(0)->getUserSock());
		}
	}
	return 0;
}


void User::kickUser(std::string toKickUser, Channel& fromChannel, ChannelContainer& channel_container) {
	if (toKickUser == getNickName()) {
		std::cerr << "Can't kick yourself." << std::endl;
		return ;
	}
	fromChannel.getSpecficUser(toKickUser)->exitChannel(fromChannel.getChannelName(), channel_container);
}

void User::inviteUser(User& toInviteUser, Channel& toChannel) {
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
}

int User::changeMode(Channel &joinedChannel, bool onoff_flag, int type, std::string arg) {
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
	if (onoff_flag) {
		if (MODE_CHECK_TYPE(type, MODE_INVITE_ONLY))
			channelMode.turnOnType(MODE_INVITE_ONLY);
		if (MODE_CHECK_TYPE(type, MODE_TOPIC_LIMIT))
			channelMode.turnOnType(MODE_TOPIC_LIMIT);
		if (MODE_CHECK_TYPE(type, MODE_CHANNEL_KEY)) {
			if (channelMode.checkType(MODE_CHANNEL_KEY))
				return 1;
			channelMode.turnOnType(MODE_CHANNEL_KEY);
			if (arg == "")
				throw Command::ErrInvalidModeParam(this->getNickName(), joinedChannel.getChannelName(), "k", "You must specify a parameter for the key mode. Syntax: <key>.");
			joinedChannel.setPassword(arg);
		}
		if (MODE_CHECK_TYPE(type, MODE_OPERATOR_GRANTED)) {
			if (arg == "")
				throw Command::ErrInvalidModeParam(this->getNickName(), joinedChannel.getChannelName(), "o", "You must specify a parameter for the op mode. Syntax: <nick>.");
			std::vector<User*> usrList = joinedChannel.getUserList();
			typedef std::vector<User*>::iterator usrIterator;
			for (usrIterator user_iter = usrList.begin();user_iter != usrList.end();user_iter++) {
				if ((*user_iter)->getNickName() == arg) {
					if (joinedChannel.isClientOpList((*user_iter)->getUserSock()))
						return 1;
					channelMode.turnOnType(MODE_OPERATOR_GRANTED);
					(*user_iter)->getSpecificChannelJoined(joinedChannel.getChannelName())->getChannelOpList().\
						push_back((*user_iter)->getUserSock());
					return 0;
				}
			}
			throw Command::ErrNoSuchNick(this->getNickName(), arg);
		}
		if (MODE_CHECK_TYPE(type, MODE_CAPACITY_LIMIT)) { // l
			if (arg == "")
				throw Command::ErrInvalidModeParam(this->getNickName(), joinedChannel.getChannelName(), "l", "You must specify a parameter for the limit mode. Syntax: <limit>.");
			int size = stringToInt(arg);
			if (size < 0)
				throw Command::ErrInvalidModeParam(this->getNickName(), joinedChannel.getChannelName(), "l", "Invalid limit mode parameter. Syntax: <limit>.");
			if (joinedChannel.getCapacity() == size) {
				return 1;
			}
			if (size == 0)
				size = 1;
			joinedChannel.setCapacity(size);
		}
	}
	else {
		if (MODE_CHECK_TYPE(type, MODE_INVITE_ONLY))
			channelMode.turnOffType(MODE_INVITE_ONLY);
		if (MODE_CHECK_TYPE(type, MODE_TOPIC_LIMIT))
			channelMode.turnOffType(MODE_TOPIC_LIMIT);
		if (MODE_CHECK_TYPE(type, MODE_CHANNEL_KEY)) {
			if (!channelMode.checkType(MODE_CHANNEL_KEY))
				return 1;
			if (arg == "")
				throw Command::ErrInvalidModeParam(this->getNickName(), joinedChannel.getChannelName(), "k", "You must specify a parameter for the key mode. Syntax: <key>.");
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
					std::vector<int>& OpList = (*user_iter)->getSpecificChannelJoined(joinedChannel.getChannelName())->getChannelOpList();
					typedef std::vector<int>::iterator OpIter;
					for (OpIter iter = OpList.begin();iter != OpList.end();iter++) {
						if (*iter == (*user_iter)->getUserSock()) {
							OpList.erase(iter);
							return 0;
						}
					}
					return 0;
				}
			}
			throw Command::ErrNoSuchNick(this->getNickName(), arg);
		}
		if (MODE_CHECK_TYPE(type, MODE_CAPACITY_LIMIT)) {
			if (joinedChannel.getCapacity() == 0)
				return 1;
			joinedChannel.setCapacity(0);
		}
	}
	return 0;
}

void User::changeUserName(std::string toChange) {
	setUserName(toChange);
}

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

std::string User::getNickName(void) const {
	return nickName;
}

void User::setNickName(std::string toChangeNick){
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

std::vector<Channel*>& User::getJoinedChannelList(void) {
	return joinedChannelList;
}

Channel* User::getSpecificChannelJoined(std::string toGet) {
	std::vector<Channel*>::iterator ch_iter = getJoinedChannelList().begin();

	for (ch_iter = getJoinedChannelList().begin();\
		ch_iter != getJoinedChannelList().end();ch_iter++) {
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

void User::sendBufClear(void) {
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