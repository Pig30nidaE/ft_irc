#include "Command.hpp"
#include "../func/User.hpp"
#include "../func/ChannelContainer.hpp"
// #include "../func/Channel.hpp"


void Command::pass(std::string pass) {
	int result;
	std::cout << "[ PASS ]" << std::endl;
	std::cout << "server_pass: " << serverPass << "\n";
	std::cout << "client_input: " << pass << "\n";
	if (serverPass.compare("123")) {
		std::cout << "fail : " << result << "\n";
		throw ERR_PASSWDMISMATCH();
	}
	else {
		std::cout << "SUCCESS : " << result << "\n";
		std::cout << "PASS SUCCESS!!\n";
	}
	std::cout << "content: " << pass << "\n";
}

void Command::nick(std::string msg, ChannelContainer cc, User &user) {
	if (user.getUserIsPass() != 1) {
		throw ERR_NEEDMOREPARAMS();
	}
	if (cc.checkNickName(msg)) {
		// err
		return ;
	}
	user.setNickName(msg);
	std::cout << "[ NICK ]" << std::endl;
	std::cout << "content: " << msg << "\n";

}

void Command::user(std::string msg, User &user) {
	int colon;

	if (user.getNickName() == "") {
		//err
		return ;
	}
	colon = msg.find(':');
	std::cout << "user colon: " << &(msg[colon]) << "\n";
	// content = msg.substr(5, end - 5);
	std::cout << "[ USER ]" << std::endl;
	std::cout << "content: " << msg << "\n";
}

void Command::join(std::string msg, ChannelContainer &cc, User &user) {
	std::cout << "[ JOIN ]" << std::endl;
	std::cout << "content: " << msg << "\n";

	// msg 에서 채널, 비밀번호 분리하기
	// 초대받음: isInvited에 true로 넣기



}

void Command::part(std::string msg, ChannelContainer &cc, User &user) {
	std::cout << "[ PART] ]" << std::endl;
	std::cout << "content: " << msg << "\n";
	// 나갈 채널이랑 채널 사유 분리하기
	// 채널 사유 출력하고 채널 나가기
	user.exitChannel(msg, cc);//인원이 0되면 채널 지워줌
}

void Command::ping(std::string msg) {
			(void)msg;
			std::cout << "[ PING ]" << std::endl;
	std::cout << "content: " << msg << "\n";
}

void Command::kick(std::string msg, ChannelContainer &cc, User &user) {
	// msg에서 toKickUser, Channel 두개 뽑아내기
	user.kickUser("toKickUser", user.getSpecificChannelJoined("채널이름", cc), cc);
	std::cout << "[ KICK ]" << std::endl;
	std::cout << "content: " << msg << "\n";
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

// ---------------test용---------------
void Command::setServerPass(std::string serverPass) { // test용
	this->serverPass = serverPass;
}

Command::Command(std::string serverPass) { // test
	this->serverPass = serverPass;
}