#include "Command.hpp"
#include "../func/User.hpp"
#include "../func/ChannelContainer.hpp"
#include "../func/Channel.hpp"


void Command::pass(std::vector<std::string> msgs) {
	size_t msgsSize;
	std::cout << "[ PASS ]" << std::endl;

	msgsSize = msgs.size();
	if (msgsSize < 2) {
		std::cout << "ERROR: NEDDMOREPARAMS\n";
		throw ERR_NEEDMOREPARAMS();
	}
	if (serverPass != msgs[1]) {
		std::cout << "ERROR: PASSWDMISMATCH\n";
		throw ERR_PASSWDMISMATCH();
	}
	else {
		std::cout << "PASS SUCCESS!!\n";
	}
}

void Command::nick(std::vector<std::string> msgs, ChannelContainer cc, User &user) {
	size_t msgsSize;
	std::cout << "[ NICK ]" << std::endl;

	msgsSize = msgs.size();
	if (msgsSize < 2) {
		std::cout << "ERROR: NEDDMOREPARMS\n";
		return ;
	}
	if (cc.checkNickName(msgs[1])) {
		std::cout << "ERROR: NICK USED\n";
		return ;
	}
	user.setNickName(msgs[1]);
	std::cout << "NICK SUCCESS!!\n";
}

std::string Command::user(std::vector<std::string> msgs, User &user) {
	size_t msgsSize;
	std::string realname;
	std::cout << "[ USER ]" << std::endl;

	msgsSize = msgs.size();
	if (msgsSize < 5) {
		std::cout << "ERROR: NEED MORE PARMS\n";
		return "";
	}
	if (user.getNickName() == "") { // pass -> nick -> user 순서 준수를 위해 체크
		std::cout << "ERROR: NEED NICK COMMAND\n";
		return "";
	}
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
	//user.setUserName(msg);
	user.setUserName(realname);
	std::string say = ":irc.local 001 nickname :Welcome to the Localnet IRC Network nickname!root@127.0.0.1\n";
	return say;
}

int Command::join(std::vector<std::string> msgs, ChannelContainer &cc, User &user) {
	std::cout << "[ JOIN ]" << std::endl;
	// msgs[0] == 채널, msgs[1] == 비번 파싱하면 적용
	
	//std::cout << "CHECKER1" << std::endl;
	int status;
	if (msgs[2].size())
		status = user.joinChannel(msgs[1], msgs[2], cc); // join 마지막 인자 없앰 (flag)
	else
		status = user.joinChannel(msgs[1], "", cc);
	return status;
	//std::cout << "CHECKER2" << std::endl;
	//여기서 user는 조인하려는 사람, 그런데 조인도 안했는데 getSpecific을 실행해서 터짐.
	//std::vector<User*> userList = user.getSpecificChannelJoined(msgs[1], cc)->getUserList(); //이밑에서 터짐.
	//typedef std::vector<User*>::iterator iter;
	//for (iter a = userList.begin();a != userList.end();a++) {
	//	std::cout << (*a)->getNickName() << std::endl;
	//	std::cout << "======" << std::endl;
	//}
	//std::cout << "CHECKER3" << std::endl;
}

void Command::part(std::vector<std::string> msgs, ChannelContainer &cc, User &user) {
	std::cout << "[ PART ]" << std::endl;
	// std::cout << "content: " << msg << "\n";
	// 나갈 채널이랑 채널 사유 분리하기
	// 채널 사유 출력하고 채널 나가기
	if (msgs[1].size())
		std::cout << user.getNickName() << " is exit by " << msgs[1] << std::endl;
	user.exitChannel(msgs[0], cc);//인원이 0되면 채널 지워줌
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
	std::cout << user.getUserName() << std::endl;
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

std::string Command::topic(Channel &channel, std::string topic) {
	// topic set한 사람 저장하기
	if (topic == "") {
		return (channel.getTopicRef().getTopic());
	}
	else {
		channel.getTopicRef().setTopic(topic);
		return "";
	}
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

// ---------------test용---------------
void Command::setServerPass(std::string serverPass) { // test용
	this->serverPass = serverPass;
}

Command::Command(std::string serverPass) { // test
	this->serverPass = serverPass;
}