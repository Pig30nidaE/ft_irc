#include "Command.hpp"
#include "../func/User.hpp"
#include "../func/ChannelContainer.hpp"
#include "../func/Channel.hpp"


void Command::pass(std::string pass) {
	int result;
	std::cout << "[ PASS ]" << std::endl;
	if (pass == "")
		std::cout << "없없음음\n";
	// if (serverPass != pass) {
	// 	std::cout << "fail : " << result << "\n";
	// 	throw ERR_PASSWDMISMATCH();
	// }
	else {
		std::cout << "PASS SUCCESS!!\n";
	}
}

void Command::nick(std::string msg, ChannelContainer cc, User &user) {
	// msg[0] == nickname
	std::cout << "[ NICK ]" << std::endl;
	if (cc.checkNickName(msg)) {
		// err
		std::cout << "nick 겹침 err\n";
		return ;
	}
	user.setNickName(msg);
	std::cout << "msg: " << msg << "\n";
	std::cout << "nick: " << user.getNickName() << std::endl;

}

std::string Command::user(std::string msg, User &user) {
	// msg[0] = username
	std::cout << "[ USER ]" << std::endl;
	if (user.getNickName() == "") { // pass -> nick -> user 순서 준수를 위해 체크
		std::cout << "nick 설정 안함err\n";
		return "";
	}
	std::cout << "msg: " << msg << "\n";


	user.setUserName(msg);
	//---------------------
	std::string say = ":irc.local 001 nickname :Welcome to the Localnet IRC Network nickname!root@127.0.0.1\n";
	return say;
}

void Command::join(std::string msg, ChannelContainer &cc, User &user) {
	std::cout << "[ JOIN ]" << std::endl;
	// msg[0] == 채널, msg[1] == 비번 파싱하면 적용
	
	user.joinChannel(msg, "", cc); // join 마지막 인자 없앰 (flag)
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
	std::cout << user.getUserName() << std::endl;
	user.kickUser(userName, *user.getSpecificChannelJoined(channelName, cc), cc);
}

void Command::invite(std::string nick, std::string channel, ChannelContainer &cc, User &user) {
	////std::vector<User*> usrList = (cc).getUserList();
	//typedef std::vector<User*>::iterator UserIter;
	//UserIter user_iter_begin = cc.getUserList().begin();
	//UserIter user_iter_end = cc.getUserList().end();
	//UserIter user_iter_find = std::find(user_iter_begin, user_iter_end, nick);

	//if (user_iter_find == user_iter_end)
	//	std::cout << "no such user." << std::endl; // ToDo: 에러메세지 바꾸삼.
	//else {
	//	User* toInvite = *user_iter_find;
	//	user.inviteUser(*toInvite, *user.getSpecificChannelJoined(channel, cc), cc);
	//}
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