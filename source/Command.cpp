#include "Command.hpp"
#include "../func/User.hpp"
#include "../func/ChannelContainer.hpp"
// #include "../func/Channel.hpp"


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

}

void Command::user(std::string msg, User &user) {
	// msg[0] = username
	std::cout << "[ USER ]" << std::endl;
	if (user.getNickName() == "") { // pass -> nick -> user 순서 준수를 위해 체크
		std::cout << "nick 설정 안함err\n";
		return ;
	}
	std::cout << "msg: " << msg << "\n";


	user.setUserName(msg);
	//---------------------
	std::string say = ":irc.local 001 nickname :Welcome to the Localnet IRC Network nickname!root@127.0.0.1";
	int len = say.length();
	// std::cout << "len: " << len << "\n";
	write(user.getUserSock(), ":irc.local 001 nickname :Welcome to the Localnet IRC Network nickname!root@127.0.0.1\n", len + 1);
	
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

void Command::ping(std::string msg, User &user) {
	std::cout << "[ PING ]" << std::endl;
	std::cout << "content: " << msg << "\n";
	std::string req = "PONG " + msg;
	write(user.getUserSock(), &req, req.length() + 1);
	std::cout << "req: " << req << "\n";
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

void Command::invite(std::string nick. std::string channel, User &user) {
	user.setIsInvite(1);
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