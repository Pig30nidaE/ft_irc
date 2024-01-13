#include "../include/Command.hpp"
#include "../include/User.hpp"
#include "../include/ChannelContainer.hpp"
#include "../include/Channel.hpp"

void Command::pass(std::vector<std::string> msgs, User &user) {
	size_t msgsSize;

	msgsSize = msgs.size();
	if (user.getUserIsPass() != 5 && user.getUserIsPass() != 4) {
		if (msgsSize < 2) {
			user.setUserIsPass(NEEDMOREPARMS);
			return ;
		}
		if (serverPass != msgs[1]) {
			user.setUserIsPass(PASSMISMATCH);
		}
		else {
			user.setUserIsPass(PASSSUCCESS);
		}
	}
	else {
		throw ErrAlreadyRegistered("PASS");
	}
}

void Command::nick(std::vector<std::string> msgs, ChannelContainer &cc, User &user) {
	size_t msgsSize;
	std::string nickname;

	nickname = user.getNickName();
	msgsSize = msgs.size();
	if (msgsSize < 2) {
		throw ErrNeedMoreParams(user.getNickName(), "NICK");
	}
	if (nickname == msgs[1]) {
		return ;
	}
	else if (cc.checkNickName(msgs[1])) {
		throw ErrNickNameInUser(msgs[1]);
	}
	else if (msgs[1][0] == '#')
		throw ErrErroneusNickName(msgs[1]);
	if (user.getUserIsPass() == 5) {
		std::vector<Channel*> joinedChannelList;
		std::vector<User*> userList;
		std::set<int> sentUsers;
		int sentsock;
		int sock;
		sock = user.getUserSock();
		joinedChannelList = user.getJoinedChannelList();
		for (std::vector<Channel*>::iterator iter = joinedChannelList.begin(); iter != joinedChannelList.end(); iter++) {
			userList = (*iter)->getUserList();
			for (std::vector<User*>::iterator uIter = userList.begin(); uIter != userList.end(); uIter++) {
				sentsock = (**uIter).getUserSock();
				if (sentsock != sock) {
					if (sentUsers.find(sentsock) == sentUsers.end()) {
						sentUsers.insert(sentsock);
						sendMsg(**uIter, NICK_REPLISE(user.getClient(), msgs[1]));
					}
				}
			}
		}
		sendMsg(user, NICK_REPLISE(user.getClient(), msgs[1]));
		user.setClient(msgs[1], user.getUserName(), user.getHostName());
	}
	user.setNickName(msgs[1]);
	return ;
}

void Command::user(std::vector<std::string> msgs, User &user) {
	size_t msgsSize;
	std::string realname;
	std::string nickname;

	if (user.getUserIsPass() != 5) {
		msgsSize = msgs.size();
		if (msgsSize < 5) {
			throw ErrNeedMoreParams(user.getNickName(), "USER");
		}
		nickname = user.getNickName();
		if (msgs[4][0] == ':') {
			for(std::vector<std::string>::iterator iter = msgs.begin()+4; iter != msgs.end(); iter++) {
				realname += *iter;
				if ((iter + 1) != msgs.end()) {
					realname += ' ';
				}
			}
			realname = &(realname[1]);
		}
		else {
			realname = msgs[4];
		}
		user.setUserName(msgs[1]);
		user.setRealName(realname);
		user.setHostName(msgs[2]);
	}
	else {
		throw ErrAlreadyRegistered("USER");
	}
}

void Command::join(std::vector<std::string> msgs, ChannelContainer &cc, User &user) {
	std::vector<std::string> channelnames;
	std::vector<std::string> keys;
	std::string key;
	size_t keySize;
	size_t index = 1;
	int msgsSize;
	int result;
	std::string nickName;
	std::string rplEndofnames;
	Channel *channel;


	msgsSize = msgs.size();
	if (msgsSize < 2) {
		throw ErrNeedMoreParams(user.getNickName(), "JOIN");
	}
	channelnames = splitComma(msgs[1]);
	if (msgsSize >= 3)
		keys = splitComma(msgs[2]);
	keySize = keys.size();
	for (std::vector<std::string>::iterator iter = channelnames.begin(); iter != channelnames.end(); iter++) {
		try
		{
			if (*iter == "")
				continue;
			else if ((*iter)[0] != '#')
				throw ErrBadChanMask((*iter));
			if (keySize >= index)
				key = keys[index - 1];
			else
				key = "";
			result = user.joinChannel((*iter), key, cc);
			if (result == ERR_CHANNELISFULL_NUM)
				throw ErrChannelIsFull(*iter, nickName);
			else if (result == ERR_BADCHANNELKEY_NUM)
				throw ErrBadChannelKey(user.getNickName(), (*iter));
			else if (result == ERR_INVITEONLYCHAN_NUM)
				throw ErrInviteOnlyChan(*iter, nickName);
			else if ((user.getJoinedChannelList()).size() > (unsigned long)cc.getUserJoinChanLimit()) {
				throw ErrTooManyChannels(*iter, nickName);
			}
			else if (result == ERR_ALREADYONCHAN)
				return ;
			else {
				channel = user.getSpecificChannelJoined(*iter);
				nickName = user.getNickName();
				rplEndofnames = RPL_NAMREPLY(nickName, (*iter));
				std::vector<User *> userList = channel->getUserList();
				for (std::vector<User *>::iterator iter = userList.begin(); iter != userList.end(); iter++) {
					rplEndofnames += ((*iter)->getNickName() + " ");
				}
				rplEndofnames += "\r\n";
				sendMsg(user, JOIN_REPLISE(user.getClient(), (*iter)) + rplEndofnames + RPL_ENDOFNAMES(nickName, (*iter)));
				Topic& joinedChanTopic = channel->getTopicRef();
				if (joinedChanTopic.getTopic().size())
					sendMsg(user, RPL_TOPIC(channel->getChannelName(), nickName, joinedChanTopic.getTopic()));
				broadcastMsg(user, channel, ":" + nickName + " JOIN :" + (*iter) + "\r\n");
			}
		}
		catch(const std::exception& e) {
			sendMsg(user, e.what());
		}
		index++;
	}
}

void Command::part(std::vector<std::string> msgs, ChannelContainer &cc, User &user) {
	std::vector<std::string> targets;
	std::string response = "";
	size_t msgsSize;
	Channel *channel = NULL;
	std::string channelname;
	std::string nickname;
	std::string username;

	msgsSize = msgs.size();
	if (msgsSize < 2) {
		throw ErrNeedMoreParams(user.getNickName(), "PART");
	}
	targets = splitComma(msgs[1]);
	nickname = user.getNickName();
	username = user.getUserName();
	if (msgsSize > 2) {
		if (msgs[2].length() != 0 && msgs[2][0] == ':')
			response = &(msgs[2][1]);
		else 
			response = msgs[2];
		if (msgsSize > 3) {
			for (size_t i = 3; i < msgsSize; i++) {
				response += (" " + msgs[i]);
			}
		}
	}
	for (std::vector<std::string>::iterator iter = targets.begin(); iter != targets.end(); iter++) {
		try
		{
			if ((*iter) == "")
				continue;
			if (user.checkChannelInList((*iter), cc) == ERR_NOSUCHCHANNEL_NUM) {
				throw ErrNoSuchChannel(user.getNickName(), *iter);
			}
			else if (!(channel = user.getSpecificChannelJoined(*iter))) {
				throw ErrNotonChannel(*iter, nickname);
			}
			channelname = channel->getChannelName();
			broadcastMsg(user, channel, PART_REPLISE(user.getClient(), channelname));
			sendMsg(user, PART_REPLISE(user.getClient(), channelname));
			user.exitChannel(*iter, cc);
		}
		catch(const std::exception& e) {
			sendMsg(user, e.what());
		}
	}
}

void Command::ping(std::vector<std::string> msgs, User &user) {

	if (msgs.size() < 2)
		throw ErrNeedMoreParams(msgs.at(0), msgs.at(1));
	sendMsg(user, "PONG " + msgs[1] + "\r\n");
}

void Command::kick(std::vector<std::string> msgs, ChannelContainer &cc, User &user) {
	if (msgs.size() < 3)
		throw ErrNeedMoreParams(msgs.at(0), msgs.at(1));
	std::string reason = "";
	std::string channelName = msgs.at(1);
	Channel* joinedChannel = user.getSpecificChannelJoined(channelName);
	
	if (!joinedChannel)
		throw ErrNoSuchChannel(user.getNickName(), channelName);
	if (!(joinedChannel->isOnTheChan(user)))
		throw ErrNotonChannel(channelName, user.getNickName());
	if (!(joinedChannel->isOperator(user)))
		throw ErrChanOPrivsNeeded(channelName);
	std::string toKick = msgs.at(2);
	if (!(joinedChannel->getSpecficUser(toKick)))
		throw ErrNoSuchNick(user.getNickName(), toKick);
	if (msgs.size() > 3) {
		reason = msgs.at(3).substr(1, msgs.at(3).length() - 1);
		broadcastMsg(user, joinedChannel, KICK_REPLY(user.getNickName(), user.getUserName(), channelName, toKick, reason));
		sendMsg(user, KICK_REPLY(user.getNickName(), user.getUserName(), channelName, toKick, reason));
	}
	else {
		broadcastMsg(user, joinedChannel, KICK_REPLY(user.getNickName(), user.getUserName(), channelName, toKick, ""));
		sendMsg(user, KICK_REPLY(user.getNickName(), user.getUserName(), channelName, toKick, ""));
	}
	user.kickUser(toKick, *user.getSpecificChannelJoined(channelName), cc);
}

void Command::invite(std::vector<std::string> msgs, ChannelContainer &cc, User &user) {
	typedef std::vector<User*>::iterator UserIter;
	std::vector<User*> usrList = cc.getUserList();
	UserIter toFind;

	if (msgs.size() < 3) {
		throw ErrNeedMoreParams(user.getNickName(), "INVITE");
	}
	std::string nickname = msgs.at(1);
	std::string channel = msgs.at(2);
	for (toFind = usrList.begin();toFind != usrList.end();toFind++) {
		if ((*toFind)->getNickName() == nickname) {
			break ;
		}
	}
	if (toFind == usrList.end()) {
		throw ErrNoSuchNick(user.getNickName(), nickname);
	}
	Channel* chan = cc.getSpecificChannelByName(channel);
	if (!chan)
		throw ErrNoSuchChannel(user.getNickName(), channel);
	if (!(chan->isOnTheChan(user)))
		throw ErrNotonChannel(channel, user.getNickName());
	if (!(chan->isOperator(user)))
		throw ErrChanOPrivsNeeded(channel);
	else {
		User* toInvite = *toFind;
		if (chan->isOnTheChan(*toInvite))
			throw ErrUserOnChannel(user.getNickName(), channel);
		user.inviteUser(*toInvite, *user.getSpecificChannelJoined(channel));
		sendMsg(user, RPL_INVITING(user.getNickName(), toInvite->getNickName(), channel));
		sendMsg(*toInvite, RPL_INVITEYOU(user.getClient(), nickname, channel));
	}
}

void Command::topic(std::vector<std::string> msgs, User& user) {
	std::string nickName = user.getNickName();
	std::string command = msgs.at(0);
	if (msgs.size() < 2) {
		throw ErrNeedMoreParams(nickName, command);
	}
	std::string chanName = msgs.at(1);
	Channel* channel = user.getSpecificChannelJoined(chanName);
	if (!channel) {
		throw ErrNotonChannel(chanName, nickName);
	}
	Topic& returnTopic = channel->getTopicRef();
	if (msgs.size() > 2) {
		Mode& mode = channel->getModeRef();
		if (mode.checkType(MODE_TOPIC_LIMIT)) {
			if (!(channel->isOperator(user))) {
				throw ErrChanOPrivsNeeded(chanName);
			}
		}
		std::string toSet = msgs.at(2);
		returnTopic.setTopic(toSet);
		sendMsg(user, RPL_TOPIC(chanName, nickName, toSet));
		broadcastMsg(user, channel, RPL_TOPIC(chanName, nickName, toSet));
	}
	else {
		std::string chanTopic = returnTopic.getTopic();
		if (chanTopic.size()) {
			sendMsg(user, RPL_TOPIC(chanName, nickName, chanTopic));
		}
		else
			sendMsg(user, RPL_NOTOPIC(chanName, nickName));
		return ;
	}
	
}

int Command::checkMode(char c) {
	if (c == 'i')
		return (MODE_INVITE_ONLY);
	else if (c == 't')
		return (MODE_TOPIC_LIMIT);
	else if (c == 'k')
		return (MODE_CHANNEL_KEY);
	else if (c == 'o')
		return (MODE_OPERATOR_GRANTED);
	else if (c == 'l')
		return (MODE_CAPACITY_LIMIT);
	else {
		return 0;
	}
}

int Command::checkModeChange(Mode &mode, char c) {
	if (c == 'i')
		return (mode.checkType(MODE_INVITE_ONLY));
	else if (c == 't')
		return (mode.checkType(MODE_TOPIC_LIMIT));
	else if (c == 'k')
		return (mode.checkType(MODE_CHANNEL_KEY));
	else if (c == 'i')
		return (mode.checkType(MODE_OPERATOR_GRANTED));
	else if (c == 'l')
		return (mode.checkType(MODE_CAPACITY_LIMIT));
	else {
		return 0;
	}
}

std::string Command::modeParsing(User &user, std::string modestring, Mode mode) {
	size_t length = modestring.length();
	std::string temp = "";
	std::string response = "";
	char target = 0;
	int flag = PLUS;
	for (unsigned long i = 0; i < length; i++) {
		try
		{
			target = modestring[i];
			if (target == '+') {
				flag = PLUS;					
			}
			else if (target == '-') {
				flag = MINUS;
			}
			else if (!checkMode(target)) {
				throw Command::ErrUnknownMode(user.getNickName(), target);
			}
			else {
				if (checkModeChange(mode, target)){
					if (flag == MINUS) {
						temp += '-';
						temp += target;
						mode.turnOffType(checkMode(target));
					}
					else if ((flag == PLUS) && (target == 'o' || target == 'k' || target == 'l')) {
						temp += '+';
						temp += target;
					}
				}
				else {
					if (flag == PLUS) {
						temp += '+';
						temp += target;
						mode.turnOnType(checkMode(target));
					}
					else if ((flag == MINUS) && (target == 'o' || target == 'k' || target == 'l')) {
						temp += '-';
						temp += target;
					}
				}
			}
		}
		catch(const std::exception& e)
		{
			sendMsg(user, e.what());
		}
	}
	flag = 0;
	length = temp.length();
	for (size_t i = 0; i < length; i++) {
		if (temp[i] == '+') {
			if (flag == PLUS) 
				continue;
			else {
				flag = PLUS;
				response += temp[i];
			}
		}
		else if (temp[i] == '-') {
			if (flag == MINUS)
				continue;
			else {
				flag = MINUS;
				response += temp[i];
			}
		}
		else {
			if (i == 0)
				response.insert(response.begin(), '+');
			response += temp[i];
		}
	}
	return response;
}

void Command::mode(std::vector<std::string> &msgs, User& user) {
	size_t msgsSize;
	Channel *channel;
	std::string response = "";

	msgsSize = msgs.size();
	if (msgsSize < 2) {
		throw ErrNeedMoreParams(user.getNickName(), msgs[0]);
	}
	else if (msgs[1][0] != '#') {
		throw ErrNoSuchNick(user.getNickName(), msgs[1]);
	}
	channel = user.getSpecificChannelJoined(msgs[1]);
	if (channel == NULL) {
		throw ErrNoSuchChannel(user.getNickName(), msgs[1]);
	}
	if (msgsSize < 3) {
		response += '+';
		if (channel->getModeRef().checkType(MODE_INVITE_ONLY))
			response += 'i';
		if (channel->getModeRef().checkType(MODE_TOPIC_LIMIT))
			response += 't';
		if (channel->getModeRef().checkType(MODE_CHANNEL_KEY))
			response += 'k';
		if (channel->getModeRef().checkType(MODE_CAPACITY_LIMIT))
			response += 'l';
		sendMsg(user, RPL_CHANNELMODEIS(user.getNickName(), msgs[1], response));
		return ;
	}
	else {
		Mode &mode = channel->getModeRef();
		response = modeParsing(user, msgs[2], mode);
		size_t length = response.length();
		int flag = 0;
		unsigned long argIndex = 2;
		std::string result = "";
		std::string args = "";
		for (size_t i = 0; i < length; i++) {
			try
			{
				if (response[i] == '-') {
					flag = 0;
					result += response [i];
				}
				else if (response[i] == '+') {
					flag = 1;
					result += response [i];
				}
				else {
					if (response[i] == 'k' || (response[i] == 'l' && flag == 1) || response[i] == 'o')
						argIndex++;
					if (argIndex <= msgsSize - 1 && (response[i] == 'k' || response[i] == 'l' || response[i] == 'o')) {
						if (!user.changeMode(*channel, flag, checkMode(response[i]), msgs[argIndex])) {
							result += response[i];
							args += msgs[argIndex] + " ";
						}
					}
					else {
						if (!user.changeMode(*channel, flag, checkMode(response[i]), "")) {
							result += response[i];
						}
					}
				}
			}
			catch(const std::exception& e) {
				sendMsg(user, e.what());
			}
		}
		flag = 0;
		for (unsigned long i = 0; i < result.length(); i++) {
			if (result[i] != '+' && result[i] != '-')
				flag = 1;
			else if (i != 0 && result[i - 1] == '+') {
				result.erase(result.begin() + (i - 1), result.begin() + i); 
				i--;
			}
			else if (i != 0 && result[i - 1] == '-') {
				result.erase(result.begin() + (i - 1), result.begin() + i); 
				i--;
			}
		}
		if (flag == 0)
			result = "";
		if (!result.empty()) {
			result = result + " " + args;
			broadcastMsg(user, channel, RPL_MODEREPLIES(user.getClient(), channel->getChannelName(), result));
			sendMsg(user, RPL_MODEREPLIES(user.getClient(), channel->getChannelName(), result));
		}
	}
}

void Command::privmsg(std::vector<std::string> msgs, User &user, ChannelContainer &cc) {
	std::vector<std::string> targets;
	std::vector<User *> userList;
	Channel *channel;
	size_t msgsSize;
	std::string nickName;
	std::string response = "";
	User *targetUser;

	msgsSize = msgs.size();
	if (msgsSize < 3) {
		throw ErrNeedMoreParams(user.getNickName(), "PRIVMSG");
	}
	targets = splitComma(msgs[1]);
	nickName = user.getNickName();
	if (msgs[2][0] == ':')
		response = &(msgs[2][1]);
	else 
		response = msgs[2];
	if (msgsSize > 3) {
		for (size_t i = 3; i < msgsSize; i++) {
			response += (" " + msgs[i]);
		}
	}
	for(std::vector<std::string>::iterator iter = targets.begin(); iter != targets.end(); iter++) {
		try
		{
			if ((*iter) == "")
				continue;
			if ((*iter)[0] == '#') {
				if (user.checkChannelInList((*iter), cc) == ERR_NOSUCHCHANNEL_NUM) {
					throw ErrNoSuchChannel(user.getNickName(), (*iter));
				}
				channel = user.getSpecificChannelJoined((*iter));
				if (!channel)
					throw ErrCannotSendToChan(nickName, (*iter));
				broadcastMsg(user, channel, PRIVMSG_REPLISE(user.getClient(), (*iter), response));
			}
			else {
				if (cc.checkClientInList((*iter)) == ERR_NOSUCHNICK_NUM)
					throw ErrNoSuchNick(user.getNickName(), (*iter));
				targetUser = cc.getUser((*iter));
				sendMsg((*targetUser), PRIVMSG_REPLISE(user.getClient(), (*iter), response));
			}
		}
		catch(const std::exception& e) {
			sendMsg(user, e.what());
		}
	}
}

void Command::quit(std::vector<std::string> msgs, User &user) {
	std::string reason = "Client exited";
	std::vector<Channel*> joinedChannel;
	std::vector<User*> userList;
	std::set<int> sentUsers;
	int sock;
	int sentsock;

	if (msgs.size() >= 2) {
		reason = "";
		for (std::vector<std::string>::iterator iter = (msgs.begin() + 1); iter != msgs.end(); iter++) {
			reason += *iter;
			if(iter + 1 != msgs.end())
				reason += ' ';
		}
		if (reason.length() > 1 && reason[0] == ':') {
			reason = &(reason[1]);
		}
	}
	sock = user.getUserSock();
	joinedChannel = user.getJoinedChannelList();
	for (std::vector<Channel*>::iterator iter = joinedChannel.begin(); iter != joinedChannel.end(); iter++) {
		userList = (*iter)->getUserList();
		for (std::vector<User*>::iterator uIter = userList.begin(); uIter != userList.end(); uIter++) {
			sentsock = (**uIter).getUserSock();
			if (sentsock != sock) {
				if (sentUsers.find(sentsock) == sentUsers.end()) {
					sentUsers.insert(sentsock);
					sendMsg(**uIter, QUIT_REPLISE_BROAD(user.getClient(), reason));
				}
			}
		}
	}
	reason = QUIT_REPLISE_SEND(user.getUserName(), user.getHostName(), reason);
	send(user.getUserSock(), reason.c_str(), reason.length() + 1, 0);
}

Command::Command(void) {}

Command::~Command() {}

Command::Command(Command &other) {
	*this = other;
}
Command Command::operator=(Command &other) {
	(void)other;
	return *this;
}

int	checkLogin(void) {
	return 1;
}

std::vector<std::string> Command::splitComma(std::string msg) {
	std::vector<std::string> targets;
	size_t start = 0;
    size_t commaPos = 0;
	std::string temp;

	 while ((commaPos = msg.find(',', start))!= std::string::npos) {
		temp = msg.substr(start, commaPos - start);
			targets.push_back(temp);
        start = commaPos + 1;
    }
	temp = msg.substr(start);
	targets.push_back(temp);
	return targets;
}

void Command::checkLastPassState(User &user) const {
	int value;

	value = user.getUserIsPass();
	if (value == PASSSUCCESS) {
		user.setUserIsPass(4);
	}
	else if (value == NEEDMOREPARMS) {
		throw ErrNeedMoreParams(user.getNickName(), "PASS");
	}
	else if (value == PASSMISMATCH) {
		throw ErrPasswdMisMatch();
	}
}

void	Command::sendMsg(User &user, const std::string msg) const {
    if (msg == "")
        return ;
	user.setSendBuf(msg);
}

void	Command::broadcastMsg(User &user, Channel *channel, const std::string msg) const {
	std::vector<User *> userList;
    int sock;

    if (!channel) {
        return ;
    }
	userList = channel->getUserList();
	for (std::vector<User*>::iterator iter = userList.begin(); iter != userList.end(); iter++) {
        sock = (**iter).getUserSock();
        if (sock != user.getUserSock()) {
            sendMsg(**iter, msg);
        }
	}
}

void Command::setServerPass(std::string serverPass) {
	this->serverPass = serverPass;
}

Command::Command(std::string serverPass) {
	this->serverPass = serverPass;
}