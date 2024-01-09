#include "Command.hpp"
#include "../func/User.hpp"
#include "../func/ChannelContainer.hpp"
#include "../func/Channel.hpp"


void Command::pass(std::vector<std::string> msgs, User &user) {
	size_t msgsSize;
	std::cout << "[ PASS ]" << std::endl;
	std::cout << msgs[1] << "/size: " << msgs[1].size() << std::endl;
	std::cout << serverPass << std::endl;

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
			std::cout << "PASS SUCCESS!!" << std::endl;
			user.setUserIsPass(PASSSUCCESS);
		}
	}
	else {
		throw ErrAlreadyRegistered("PASS");
	}
}

int Command::nick(std::vector<std::string> msgs, ChannelContainer cc, User &user) {
	size_t msgsSize;
	std::string nickname;
	std::cout << "[ NICK ]" << std::endl;

	nickname = user.getNickName();
	msgsSize = msgs.size();
	//std::cout << msgsSize << std::endl;
	if (msgsSize < 2) {
		std::cout << "ERROR: NEDDMOREPARMS\n";
		throw ErrNeedMoreParams(user.getNickName(), "NICK");
	}
	if (nickname == msgs[1]) { // 본인 닉넴으로 변경할 때는 그냥 넘김
		return 1;
	}
	else if (cc.checkNickName(msgs[1])) {
		std::cout << "ERROR: NICK USED\n";
		throw ErrNickNameInUser(msgs[1]);
	}
	if (nickname != "") // 회원가입 할 때는 안보냄, !!!!replise 생각하기!!!!
		sendMsg(user, nickname + " NICK :" + msgs[1] + "\r\n");
	user.setNickName(msgs[1]);
	std::cout << "NICK SUCCESS!!\n";
	return 1;
}

void Command::user(std::vector<std::string> msgs, User &user) {
	size_t msgsSize;
	std::string realname;
	std::string nickname;
	std::cout << "[ USER ]" << std::endl;

	if (user.getUserIsPass() != 5) {
		msgsSize = msgs.size();
		if (msgsSize < 5) {
			std::cout << "ERROR: NEDDMOREPARAMS\n";
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
		std::cout << "username:" << msgs[1] << "\n";
		std::cout << "realname:" << realname << "\n";
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

	//최대 가입 가능 채널 수 5
	// Ban 당하는 것에 대한 처리 해야됨!!
	// 입장하면 topic을 알려줘야됨!!
	std::cout << "[ JOIN ]" << std::endl;

	msgsSize = msgs.size();
	if (msgsSize < 2) { // channelName도 없으면 에러
		std::cout << "ERROR: NEDDMOREPARMS\n";
		throw ErrNeedMoreParams(user.getNickName(), "JOIN");
	}
	channelnames = splitComma(msgs[1]);
	keys = splitComma(msgs[2]);
	keySize = keys.size();
	for (std::vector<std::string>::iterator iter = channelnames.begin(); iter != channelnames.end(); iter++) {
		try
		{
			std::cout << "[" << index << "]" << " " << *iter << "\n";
			if (*iter == "")
				continue;
			else if ((*iter)[0] != '#')
				throw ErrBadChanMask((*iter));
			if (keySize >= index) // 순서에 key가 있음
				key = keys[index - 1];
			else // key가 부족함
				key = "";
			result = user.joinChannel((*iter), key, cc);
			if (result == ERR_CHANNELISFULL_NUM)
				throw ErrChannelIsFull(*iter, nickName);
			else if (result == ERR_BADCHANNELKEY_NUM)
				throw ErrBadChannelKey(user.getNickName(), (*iter));
			else if (result == ERR_INVITEONLYCHAN_NUM)
				throw ErrInviteOnlyChan(*iter, nickName);
			else if ((user.getJoinedChannelList(cc)).size() > cc.getUserJoinChanLimit()) { // 유저 최대 입장 제한 5
				throw ErrTooManyChannels(*iter, nickName); //최대 가입 가능한 채널 수 넘으면 에러
			}
			else {
				//Channel& joinedChan = 
				channel = user.getSpecificChannelJoined((*iter), cc);
				nickName = user.getNickName();
				Topic& joinedChanTopic = channel->getTopicRef();
				if (joinedChanTopic.getTopic().size())
					sendMsg(user, RPL_TOPIC(channel->getChannelName(), nickName, joinedChanTopic.getTopic()));
				// !!namereply operator 다시 봐야할듯!!
				rplEndofnames = RPL_NAMREPLY(nickName, (*iter));
				std::vector<User *> userList = channel->getUserList();
				for (std::vector<User *>::iterator iter = userList.begin(); iter != userList.end(); iter++) {
					rplEndofnames += ((*iter)->getNickName() + " ");
				}
				rplEndofnames += "\r\n";
				sendMsg(user, JOIN_REPLISE(user.getClient(), (*iter)) + rplEndofnames + RPL_ENDOFNAMES(nickName, (*iter)));
				broadcastMsg(user, channel, ":" + nickName + " JOIN :" + (*iter) + "\r\n");
			}
		}
		catch(const std::exception& e)
		{
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
	std::cout << "[ PART ]" << std::endl;

	msgsSize = msgs.size();
	if (msgsSize < 2) {
		throw ErrNeedMoreParams(user.getNickName(), "PART");
	}
	targets = splitComma(msgs[1]);
	nickname = user.getNickName();
	username = user.getUserName();
	if (msgsSize > 2) { // 떠나는 이유가 있을 때
		if (msgs[2].length() != 0 && msgs[2][0] == ':') // 보낼 메시지 가공
			response = &(msgs[2][1]);
		else 
			response = msgs[2];
		if (msgsSize > 3) {
			for (int i = 3; i < msgsSize; i++) {
				response += (" " + msgs[i]); // 공백으로 쪼개진 문자열 붙여줌
			}
		}
	}
	for (std::vector<std::string>::iterator iter = targets.begin(); iter != targets.end(); iter++) {
		try
		{
			std::cout << " [" << *iter << "]\n";
			if ((*iter) == "")
				continue;
			if (msgsSize > 2) { // reason이 있다면
			}
			if (user.checkChannelInList((*iter), cc) == ERR_NOSUCHCHANNEL_NUM) {
				throw ErrNoSuchChannel(user.getNickName(), *iter);
			}
			else if (!(channel = user.getSpecificChannelJoined((*iter), cc))) {
				throw ErrNotonChannel(*iter, nickname);
			}
			channelname = channel->getChannelName();
			broadcastMsg(user, channel, PART_REPLISE(user.getClient(), channelname));
			sendMsg(user, PART_REPLISE(user.getClient(), channelname));

			if (user.exitChannel(*iter, cc) == ERR_NOTONCHANNEL_NUM) //인원이 0되면 채널 지워줌
				throw ErrNotonChannel(*iter, nickname); //내가 가입한 채널 중에 없음
			else {
				if (msgsSize > 2) {
					broadcastMsg(user, channel, PART_REASON_REPLISE(user.getClient(), channelname, response));
					sendMsg(user, PART_REASON_REPLISE(user.getClient(), channelname, response));
				}
				else {
					broadcastMsg(user, channel, PART_REPLISE(user.getClient(), channelname));
					sendMsg(user, PART_REPLISE(user.getClient(), channelname));
				}
			}
		}
		catch(const std::exception& e)
		{
			sendMsg(user, e.what());
			std::cerr << e.what() << '\n';
		}
	}
}

void Command::ping(std::vector<std::string> msgs, User &user) {
	std::cout << "[ PING ]" << std::endl;

	if (msgs.size() < 2)
		throw ErrNeedMoreParams(msgs.at(0), msgs.at(1));
	sendMsg(user, "PONG " + msgs[1] + "\r\n");
}

void Command::kick(std::vector<std::string> msgs, ChannelContainer &cc, User &user) {
	std::cout << "[ KICK ]" << std::endl;
	//std::cout << "msg: " << msg << "\n";
	//int spaceIndex = msg.find(' ');
	//std::string channelName = msg.substr(0, spaceIndex);
	if (msgs.size() < 3)
		throw ErrNeedMoreParams(msgs.at(0), msgs.at(1));
	std::string channelName = msgs.at(1);
	Channel* joinedChannel = user.getSpecificChannelJoined(channelName, cc);
	
	if (!joinedChannel)
		throw ErrNoSuchChannel(user.getNickName(), channelName);
	if (!(joinedChannel->isOnTheChan(user)))
		throw ErrNotonChannel(channelName, user.getNickName());
	if (!(joinedChannel->isOperator(user)))
		throw ErrChanOPrivsNeeded(channelName);
	//std::string userName = msg.substr(spaceIndex + 1, msg.length());
	std::string toKick = msgs.at(2);
	if (!(joinedChannel->getSpecficUser(toKick)))
		throw ErrNoSuchNick(user.getNickName(), toKick);
	//std::cout << "username: " << userName << "\n";
	//std::cout << "channelName: " << channelName << "\n";
	//std::cout << user.getUserName() << std::endl;
	user.kickUser(toKick, *user.getSpecificChannelJoined(channelName, cc), cc);
	sendMsg(user, KICK_REPLY(user.getNickName(), user.getUserName(), channelName, toKick));
}

void Command::invite(std::string nick, std::string channel, ChannelContainer &cc, User &user) {
	//std::vector<User*> usrList = (cc).getUserList();
	typedef std::vector<User*>::iterator UserIter;
	std::vector<User*> usrList = cc.getUserList();
	//std::string toFind;
	UserIter toFind;
	for (toFind = usrList.begin();toFind != usrList.end();toFind++) {
		if ((*toFind)->getNickName() == nick) {
			toFind = toFind;
			break ;
		}
	}
	if (toFind == usrList.end()) {
		std::cerr << "no such user." << std::endl; //err메세지 바꾸기
		return ;
	}
	else {
		User* toInvite = *toFind;
		user.inviteUser(*toInvite, *user.getSpecificChannelJoined(channel, cc), cc);
	}
}

void Command::topic(ChannelContainer&cc, std::vector<std::string> msgs, User& user) {
	std::string nickName = user.getNickName();
	std::string command = msgs.at(0);
	if (msgs.size() < 2) {
		throw ErrNeedMoreParams(nickName, command);
	}
	std::string chanName = msgs.at(1);
	Channel* channel = user.getSpecificChannelJoined(chanName, cc);
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

static int checkMode(char c) {
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

static int checkModeChange(Mode &mode, char c) {
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
	for (int i = 0; i < length; i++) {
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
	for (int i = 0; i < length; i++) {
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

void Command::mode(std::vector<std::string> &msgs, User& user, ChannelContainer& cc) {
	size_t msgsSize;
	Channel *channel;
	std::string response = "";
	std::cout << "[ MODE ]" << std::endl;

	msgsSize = msgs.size();
	if (msgsSize < 2) {
		throw ErrNeedMoreParams(user.getNickName(), msgs[0]);
	}
	else if (msgs[1][0] != '#') { // nick은 안받음 채널에대한 mode만 구현
		throw ErrNoSuchNick(user.getNickName(), msgs[1]);
	}
	channel = user.getSpecificChannelJoined(msgs[1], cc);
	if (channel == NULL) { // 채널 못찾았을 때
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
		int argIndex = 2;
		std::string result = "";
		std::string args = "";
		for (size_t i = 0; i < length; i++) {
			try
			{
				if (response[i] == '-') {
					flag = 0; // off
					result += response [i];
				}
				else if (response[i] == '+') {
					flag = 1; // on
					result += response [i];
				}
				else {
					if (response[i] == 'k' || (response[i] == 'l' && flag == 1) || response[i] == 'o')
						argIndex++;
					if (argIndex <= msgsSize - 1 && (response[i] == 'k' || response[i] == 'l' || response[i] == 'o')) {
						if (!user.changeMode(cc, *channel, flag, checkMode(response[i]), msgs[argIndex])) {
							result += response[i];
							args += msgs[argIndex] + " ";
						}
					}
					else {
						if (!user.changeMode(cc, *channel, flag, checkMode(response[i]), "")) {
							result += response[i];
						}
					}
				}
			}
			catch(const std::exception& e)
			{
				sendMsg(user, e.what());
			}
		}
		length = result.length();
		flag = 0;
		for (int i = 0; i < length; i++) {
			if (result[i] != '+' && result[i] != '-')
				flag = 1;
			else if (i != 0 && result[i - 1] == '+') {
				result.erase(result.begin() + (i - 1), result.begin() + i); 
			}
			else if (i != 0 && result[i - 1] == '-') {
				result.erase(result.begin() + (i - 1), result.begin() + i); 
			}
		}
		if (flag == 0)
			result = "";
		if (!result.empty()) {
			result = result + " " + args;
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
	std::cout << "[PRIVMSG]\n";

	msgsSize = msgs.size();
	if (msgsSize < 3) {
		throw ErrNeedMoreParams(user.getNickName(), "PRIVMSG"); // 임시 에러
	}
	targets = splitComma(msgs[1]);
	nickName = user.getNickName();
	if (msgs[2][0] == ':') // 보낼 메시지 가공
		response = &(msgs[2][1]);
	else 
		response = msgs[2];
	if (msgsSize > 3) {
		for (int i = 3; i < msgsSize; i++) {
			response += (" " + msgs[i]); // 공백으로 쪼개진 문자열 붙여줌
		}
	}
	for(std::vector<std::string>::iterator iter = targets.begin(); iter != targets.end(); iter++) {
		try
		{
			if ((*iter) == "")
				continue;
			if ((*iter)[0] == '#') { // 채널 일때
				if (user.checkChannelInList((*iter), cc) == ERR_NOSUCHCHANNEL_NUM) {
					throw ErrNoSuchChannel(user.getNickName(), (*iter));
				}
				channel = user.getSpecificChannelJoined((*iter), cc);
				if (!channel)
					throw ErrCannotSendToChan(nickName, (*iter));
				broadcastMsg(user, channel, PRIVMSG_REPLISE(user.getClient(), (*iter), response));
			}
			else { // 클라 일떄
				if (cc.checkClientInList((*iter)) == ERR_NOSUCHNICK_NUM)
					throw ErrNoSuchNick(user.getNickName(), (*iter));
				targetUser = cc.getUser((*iter));
				sendMsg((*targetUser), PRIVMSG_REPLISE(user.getClient(), (*iter), response));
			}
		}
		catch(const std::exception& e)
		{
			sendMsg(user, e.what());
		}
	}
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

int	checkLogin(User User) {
	return 1;
}

std::vector<std::string> Command::splitComma(std::string msg) {
	std::vector<std::string> targets;
	size_t start = 0;
    size_t commaPos = 0;
	std::string temp;

	 while ((commaPos = msg.find(',', start))!= std::string::npos) { // ',' 기준으로 나
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
		std::cout << "ERROR: NEDDMOREPARAMS\n";
		throw ErrNeedMoreParams(user.getNickName(), "PASS");
	}
	else if (value == PASSMISMATCH) {
		std::cout << "ERROR: PASSWDMISMATCH\n";
		throw ErrPasswdMisMatch();
	}
}

void	Command::sendMsg(User &user, const std::string msg) const {
    if (msg == "")
        return ;
	user.setSendBuf(msg);
	// write(user.getUserSock(), msg.c_str(), msg.length() + 1);
}

void	Command::broadcastMsg(User &user, Channel *channel, const std::string msg) const {
	std::vector<User *> userList;
    int sock; //test
    std::cout << "[BroadCastMsg]\n";

    if (!channel) {
        std::cout << "channel is NULL\n";
        return ; // err 던지기
    }
	userList = channel->getUserList();
	for (std::vector<User*>::iterator iter = userList.begin(); iter != userList.end(); iter++) {
        sock = (**iter).getUserSock(); // test
        if (sock != user.getUserSock()) {
			std::cout << "[BROAD] send to " << sock << "\n";// test
            sendMsg(**iter, msg);
        }
	}
}

// ---------------test용---------------
void Command::setServerPass(std::string serverPass) { // test용
	this->serverPass = serverPass;
}

Command::Command(std::string serverPass) { // test
	this->serverPass = serverPass;
}