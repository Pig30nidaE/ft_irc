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
		std::cout << "realname:" << realname << "\n";
		user.setUserName(realname);
	}
	else {
		throw ErrAlreadyRegistered("USER");
	}
}

void Command::join(std::vector<std::string> msgs, ChannelContainer &cc, User &user) {
	int result;
	int msgsSize;
	Channel *channel;
	std::string nickName;
	std::cout << "[ JOIN ]" << std::endl;
	// Ban 당하는 것에 대한 처리 해야됨!!
	// 입장하면 topic을 알려줘야됨!!
	// 채널명 확인 요함 (ex: #이 붙어있는지)

	msgsSize = msgs.size();
	if (msgsSize < 2) { // channelName도 없으면 에러
		std::cout << "ERROR: NEDDMOREPARMS\n";
		throw ErrNeedMoreParams("JOIN");
	}
	else if ((user.getJoinedChannelList(cc)).size() >= 1) { // 최대 가입 가능한 채널 수 1
		throw ErrTooManyChannels(msgs[1]); //최대 가입 가능한 채널 수 넘으면 에
	}
	if (msgsSize >= 3) // Input에 비밀번호까지 있는지, 그 이후는 걍 무시
		result = user.joinChannel(msgs[1], msgs[2], cc); // key가 있을 때
	else // 비번 없이 채널 명만 들어왔을 떄
		result = user.joinChannel(msgs[1], "", cc); // key가 없을 때

	switch (result) // joinChannel의 리턴값 수정함 memo.txt. 밑에 리턴값에 따른 에러 명세 해놓음
	{
	case ERR_CHANNELISFULL:
		throw ErrChannelIsFull(msgs[1]);
		break;
	
	case ERR_BADCHANNELKEY:
		throw ErrBadChannelKey(msgs[1]);
		break;

	case ERR_INVITEONLYCHAN:
		throw ErrInviteOnlyChan(msgs[1]);
		break;
	default:
		break;
	}
	nickName = user.getNickName();
	sendMsg(user, ":" + nickName + "!root@127.0.0.1 JOIN :#test\n:irc.local 353 " + nickName + " = #test :@" +nickName+"\n:irc.local 366 " + nickName + " #test :End of /NAMES list.\n");
	channel = user.getSpecificChannelJoined(msgs[1], cc);
	// channel의 현재 인원 확인용
	std::cout << "------channel user list 확인용-------\n";
	std::vector<User*> userList = user.getSpecificChannelJoined(msgs[1], cc)->getUserList(); //이밑에서 터짐.
	typedef std::vector<User*>::iterator iter;
	for (iter a = userList.begin();a != userList.end();a++) {
		std::cout << (*a)->getNickName() << std::endl;
		std::cout << "======" << std::endl;
	}
	// ---------------------
	broadcastMsg(user, channel, ":" + nickName + "!root@127.0.0.1 JOIN :" + msgs[1] + "\n");
}

void Command::part(std::vector<std::string> msgs, ChannelContainer &cc, User &user) {
	std::cout << "[ PART ]" << std::endl;
	// std::cout << "content: " << msg << "\n";
	// 나갈 채널이랑 채널 사유 분리하기
	// 채널 사유 출력하고 채널 나가기
	if (msgs.size() < 2) {
		std::cout << "need channel name." << std::endl;
		return ;
	}
	if (msgs.size() > 2)
		std::cout << user.getNickName() << " is exit by " << msgs[2] << std::endl;
	user.exitChannel(msgs[1], cc);//인원이 0되면 채널 지워줌
}

std::string Command::ping(std::string msg, User &user) {
	std::cout << "[ PING ]" << std::endl;
	std::cout << "content: " << msg << "\n";
	std::string req = "PONG " + msg;
	std::cout << "req: " << req << "\n";
	return req;
}

void Command::kick(std::vector<std::string> msgs, ChannelContainer &cc, User &user) {
	std::cout << "[ KICK ]" << std::endl;
	//std::cout << "msg: " << msg << "\n";
	//int spaceIndex = msg.find(' ');
	//std::string channelName = msg.substr(0, spaceIndex);
	std::string channelName = msgs[0];
	//std::string userName = msg.substr(spaceIndex + 1, msg.length());
	std::string userName = msgs[1];
	//std::cout << "username: " << userName << "\n";
	//std::cout << "channelName: " << channelName << "\n";
	//std::cout << user.getUserName() << std::endl;
	user.kickUser(userName, *user.getSpecificChannelJoined(channelName, cc), cc);
}

void Command::invite(std::string nick, std::string channel, ChannelContainer &cc, User &user) {
	//std::vector<User*> usrList = (cc).getUserList();
	typedef std::vector<User*>::iterator UserIter;
	std::vector<User*> usrList = cc.getUserList();
	//std::string toFind;
	UserIter toFind;
	for (UserIter user_iter = usrList.begin();user_iter != usrList.end();user_iter++) {
		if ((*user_iter)->getNickName() == nick) {
			toFind = user_iter;
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
	if (&channel == nullptr) {
		std::cerr << "no such channel." << std::endl;
		return "";
	}
	Mode& mode = channel.getModeRef();
	if (mode.checkType(MODE_TOPIC_LIMIT)) {
		if (user.getIsOperator() == false) {
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

    if (channel == nullptr) {
        std::cout << "channel is NULL\n";
        return ; // err 던지기
    }
    std::cout << "user sock: " << user.getUserSock() << "\n";; //test 
	userList = channel->getUserList();
	for (std::vector<User*>::iterator iter = userList.begin(); iter != userList.end(); iter++) {
        sock = (**iter).getUserSock(); // test
        std::cout << "iter sock: " << sock << "\n";// test
        if (sock != user.getUserSock()) {
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