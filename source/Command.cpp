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
		throw ErrNeedMoreParams("NICK");
	}
	if (nickname == msgs[1]) { // 본인 닉넴으로 변경할 때는 그냥 넘김
		return 1;
	}
	else if (cc.checkNickName(msgs[1])) {
		std::cout << "ERROR: NICK USED\n";
		throw ErrNickNameInUser(msgs[1]);
	}
	if (nickname != "") // 회원가입 할 때는 안보냄
		sendMsg(user, nickname + " NICK :" + msgs[1] + '\n');
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
			throw ErrNeedMoreParams("USER");
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
	}
	else {
		throw ErrAlreadyRegistered("USER");
	}
}

//void Command::join(std::vector<std::string> msgs, ChannelContainer &cc, User &user) {
//	if (msgs.size() < 2) {
//		throw ErrNeedMoreParams(msgs.at(0));
//	}
//	std::string chanName = msgs.at(1);
//	std::string enteredPw = "";
//	if (msgs.size() > 2)
//		enteredPw = msgs.at(2);
//	if (user.getJoinedChannelList(cc).size() == cc.getUserJoinChanLimit()) {
//		return sendMsg(user, ERR_TOOMANYCHANNELS(msgs.at(1)));
//	}
//	try {
//		int joinStatus = user.joinChannel(msgs.at(1), enteredPw, cc);
//		if (joinStatus == ERR_BADCHANNELKEY_NUM)
//			throw ErrBadChannelKey(chanName);
//		else if (joinStatus == ERR_CHANNELISFULL_NUM)
//			throw ErrChannelIsFull(chanName);
//		else if (joinStatus == ERR_INVITEONLYCHAN_NUM)
//			throw ErrInviteOnlyChan(chanName);
//		else {// SUCCESS
//			Channel& joinedChan = *user.getSpecificChannelJoined(chanName, cc);
//			//if (joinStatus == JOINCHANNEL) {
//			Topic& joinedChanTopic = joinedChan.getTopicRef();
//			if (joinedChanTopic.getTopic().size())
//				sendMsg(user, RPL_TOPIC(chanName, joinedChanTopic.getTopic()));
//			typedef std::vector<User*>::iterator UserIter;
//			std::vector<User*> &userList = joinedChan.getUserList();
//			std::string user_seq = "";
//			UserIter iter;
//			for (iter = userList.begin();iter != userList.end();iter++) {
//				if ((*iter)->getIsOperator())
//					user_seq += ("@" + (*iter)->getNickName());
//				else
//					user_seq += (*iter)->getNickName();
//			}
//			sendMsg(user, RPL_NAMREPLY(user.getNickName(), user_seq, chanName));
//			sendMsg(user, RPL_ENDOFNAMES(user.getNickName(), chanName));
//			//}
//			//else { // CreateChan

//			//}
//		}
//	}
//	catch(const std::exception &e) {
//		sendMsg(user, e.what());
//	}
//}




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
		throw ErrNeedMoreParams("JOIN");
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
				throw ErrChannelIsFull((*iter));
			else if (result == ERR_BADCHANNELKEY_NUM)
				throw ErrBadChannelKey((*iter));
			else if (result == ERR_INVITEONLYCHAN_NUM)
				throw ErrInviteOnlyChan((*iter));
			else if ((user.getJoinedChannelList(cc)).size() > cc.getUserJoinChanLimit()) { // 유저 최대 입장 제한 5
				throw ErrTooManyChannels((*iter)); //최대 가입 가능한 채널 수 넘으면 에러
			}
			else {
				//Channel& joinedChan = 
				channel = user.getSpecificChannelJoined((*iter), cc);
				nickName = user.getNickName();
				Topic& joinedChanTopic = channel->getTopicRef();
				if (joinedChanTopic.getTopic().size())
					sendMsg(user, RPL_TOPIC(channel->getChannelName(), joinedChanTopic.getTopic()));
				rplEndofnames = RPL_NAMREPLY(nickName, (*iter));
				std::vector<User *> userList = channel->getUserList();
				for (std::vector<User *>::iterator iter = userList.begin(); iter != userList.end(); iter++) {
					rplEndofnames += ((*iter)->getNickName() + " ");
				}
				rplEndofnames += "\n";
				sendMsg(user, JOIN_REPLISE(nickName,user.getUserName(), (*iter)) + rplEndofnames + RPL_ENDOFNAMES(nickName, (*iter)));
				broadcastMsg(user, channel, ":" + nickName + " JOIN :" + (*iter) + "\n");
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
		throw ErrNeedMoreParams("PART");
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
				throw ErrNoSuchChannel(*iter);
			}
			else if (!(channel = user.getSpecificChannelJoined((*iter), cc))) {
				throw ErrNotonChannel(*iter);
			}
			channelname = channel->getChannelName();
			broadcastMsg(user, channel, PART_REPLISE(nickname, username, channelname));
			sendMsg(user, PART_REPLISE(nickname, username, channelname));

			if (user.exitChannel(*iter, cc) == ERR_NOTONCHANNEL_NUM) //인원이 0되면 채널 지워줌
				throw ErrNotonChannel(*iter); //내가 가입한 채널 중에 없음
			else {
				if (msgsSize > 2) {
					broadcastMsg(user, channel, PART_REASON_REPLISE(nickname, username, channelname, response));
					sendMsg(user, PART_REASON_REPLISE(nickname, username, channelname, response));
				}
				else {
					broadcastMsg(user, channel, PART_REPLISE(nickname, username, channelname));
					sendMsg(user, PART_REPLISE(nickname, username, channelname));
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
		throw ErrNeedMoreParams("PING");
	sendMsg(user, "PONG " + msgs[1]);
}

void Command::kick(std::vector<std::string> msgs, ChannelContainer &cc, User &user) {
	std::cout << "[ KICK ]" << std::endl;
	//std::cout << "msg: " << msg << "\n";
	//int spaceIndex = msg.find(' ');
	//std::string channelName = msg.substr(0, spaceIndex);
	if (msgs.size() < 3)
		throw ErrNeedMoreParams(msgs.at(0));
	std::string channelName = msgs.at(1);
	Channel* joinedChannel = user.getSpecificChannelJoined(channelName, cc);
	
	if (!joinedChannel)
		throw ErrNoSuchChannel(channelName);
	if (!(joinedChannel->isOnTheChan(user)))
		throw ErrNotonChannel(channelName);
	if (!(joinedChannel->isOperator(user)))
		throw ErrChanOPrivsNeeded(channelName);
	//std::string userName = msg.substr(spaceIndex + 1, msg.length());
	std::string toKick = msgs.at(2);
	if (!(joinedChannel->getSpecficUser(toKick)))
		throw ErrNoSuchNick(toKick);
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

std::string Command::topic(ChannelContainer&cc, Channel &channel, std::string topic, User& user) {
	// topic set한 사람 저장하기
	std::stringstream ss;

	// Channel* joinedChannel = user.getSpecificChannelJoined\
	// 	(channel.getChannelName(), cc);
	//if (!channel) {
	//	std::cerr << "no such channel." << std::endl;
	//	return "";
	//}
	Mode& mode = channel.getModeRef();
	if (mode.checkType(MODE_TOPIC_LIMIT)) {
		if (channel.isOperator(user) == false) {
			std::cerr << "only operator can change topic." << std::endl;
			return "";
		}
	}
	ss << "channel :";
	if (!topic.size()) {
		std::string returnTopic = channel.getTopicRef().getTopic();
		if (!returnTopic.size()) {
			ss << "No topic is set" << std::endl;
		}
		else {
			ss << returnTopic << std::endl;
		}
			return (ss.str());
	}
	else {
		channel.getTopicRef().setTopic(topic);
		ss << channel.getTopicRef().getTopic() << std::endl;
		return ss.str();
	}
	// if (!topic.size())
	// 	user.topicCommand(cc, channel.getChannelName());
	// else
	// 	user.topicCommand(cc, channel.getChannelName(), topic);

}

void Command::mode(std::vector<std::string> msgs, User& user, ChannelContainer& cc) { //LSH
	std::cout << "[ MODE ]" << std::endl;
	//일단 /MODE <channel> [+/-] 이런식으로 테스트, MODE를 바꾸려면 직접 수정.
	// MODE +k-k+k 와 같이 여러번 들어오는 것도 가능함. 일단은 하나만 들어온다고 가정함.
	bool flag;
	if (msgs.at(2)[0] == '+')
		flag = true;
	else
		flag = false;
	std::string type = msgs[2].substr(1, msgs[2].size() - 1);
	//std::cout << type << std::endl;
	if (type == "i")
		user.changeMode(cc, msgs.at(1), flag, MODE_INVITE_ONLY);
	else if (type == "t")
		user.changeMode(cc, msgs.at(1), flag, MODE_TOPIC_LIMIT);
	else if (type == "k") {
			if (msgs.size() > 3) {
				user.changeMode(cc, msgs.at(1), flag, MODE_CHANNEL_KEY, msgs.at(3).c_str());	
			}
			else {
				std::cout << "ERROR" << std::endl;
			}
		}
		//if (flag && msgs.size() == 4) { // 나중에 확인하기. == 4가 맞는가?
		//	user.changeMode(cc, msgs.at(1), flag, MODE_CHANNEL_KEY, msgs.at(3).c_str());
		//	std::cout << 1;
		//}
		//else {
		//	std::cout << "need passwd" << std::endl;
		//	std::cout << 2;
		//}
	else if (type == "o") {
		if (msgs.size() > 3)
			user.changeMode(cc, msgs.at(1), flag, MODE_OPERATOR_GRANTED, msgs.at(3).c_str());
		else {
			std::cout << "ERROR" << std::endl;
		}
	}
	else if (type == "l") {
		if (flag) {
			if (msgs.size() > 3)
				user.changeMode(cc, msgs.at(1), flag, MODE_CAPACITY_LIMIT, std::atoi(msgs.at(3).c_str()));
			else {
				std::cout << "ERROR" << std::endl;
			}
		}
		else
			user.changeMode(cc, msgs.at(1), flag, MODE_CAPACITY_LIMIT);
	}
	else
		std::cout << "invalid option." << std::endl;
	//user.changeMode(cc, user.getSpecificChannelJoined(channel_name, cc),\
	//	onoff_flag, )
}

// PRIVMSG test1,,test2 hello 도 보내져야됨
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
		throw ErrNeedMoreParams("PRIVMSG"); // 임시 에러
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
					throw ErrNoSuchChannel((*iter));
				}
				channel = user.getSpecificChannelJoined((*iter), cc);
				if (!channel)
					throw ErrCannotSendToChan(nickName, (*iter));
				broadcastMsg(user, channel, PRIVMSG_REPLISE(nickName, user.getUserName(), (*iter), response));
			}
			else { // 클라 일떄
				if (cc.checkClientInList((*iter)) == ERR_NOSUCHNICK_NUM)
					throw ErrNoSuchNick((*iter));
				targetUser = cc.getUser((*iter));
				sendMsg((*targetUser), PRIVMSG_REPLISE(nickName, user.getUserName(), (*iter), response));
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
		throw ErrNeedMoreParams("PASS");
	}
	else if (value == PASSMISMATCH) {
		std::cout << "ERROR: PASSWDMISMATCH\n";
		throw ErrPasswdMisMatch();
	}
}

void	Command::sendMsg(User &user, const std::string &msg) const {
    if (msg == "")
        return ;
	user.setSendBuf(msg);
	// write(user.getUserSock(), msg.c_str(), msg.length() + 1);
}

void	Command::broadcastMsg(User &user, Channel *channel, const std::string &msg) const {
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