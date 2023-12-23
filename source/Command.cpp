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
	user.setUserName(realname);
	std::string say = ":irc.local 001 nickname :Welcome to the Localnet IRC Network nickname!root@127.0.0.1\n";
	return say;
}

void Command::join(std::string msg, ChannelContainer &cc, User &user) {
	std::cout << "[ JOIN ]" << std::endl;
	// msg[0] == 채널, msg[1] == 비번 파싱하면 적용
	
	user.joinChannel(msg, "", cc, false);
}

void Command::part(std::string msg, ChannelContainer &cc, User &user) {
	std::cout << "[ PART] ]" << std::endl;
	// std::cout << "content: " << msg << "\n";
	// 나갈 채널이랑 채널 사유 분리하기
	// 채널 사유 출력하고 채널 나가기
	user.exitChannel(msg, cc);//인원이 0되면 채널 지워줌
}

std::string Command::ping(std::string msg, User &user) {
	std::cout << "[ PING ]" << std::endl;
	std::cout << "content: " << msg << "\n";
	std::string req = "PONG " + msg;
	std::cout << "req: " << req << "\n";
	return req;
}

void Command::kick(std::string msg, ChannelContainer &cc, User &user) {
	std::cout << "[ KICK ]" << std::endl;
	std::cout << "msg: " << msg << "\n";
	int spaceIndex = msg.find(' ');
	std::string channelName = msg.substr(0, spaceIndex);
	std::string userName = msg.substr(spaceIndex + 1, msg.length());
	std::cout << "username: " << userName << "\n";
	std::cout << "channelName: " << channelName << "\n";
	user.kickUser(userName, *user.getSpecificChannelJoined(channelName, cc), cc);
}

void Command::invite(std::string nick, std::string channel, User &user) {
	// user.setIsInvite(1);
	return ;
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

void Command::mode(std::string channel, std::string flag, std::string parm) {

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