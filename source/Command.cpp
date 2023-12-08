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

void Command::nick(std::string msg, User &user) {
	if (user.getUserIsPass() != 1) {
		throw ERR_NEEDMOREPARAMS();
	}
	// 	보류
	std::cout << "[ NICK ]" << std::endl;
	std::cout << "content: " << msg << "\n";

}

void Command::user(std::string msg) {
	int colon;

	colon = msg.find(':');
	std::cout << "user colon: " << &(msg[colon]) << "\n";
	// content = msg.substr(5, end - 5);
	std::cout << "[ USER ]" << std::endl;
	std::cout << "content: " << msg << "\n";
}

void Command::join(std::string msg) {
			std::cout << "[ JOIN ]" << std::endl;
			(void)msg;
	std::cout << "content: " << msg << "\n";
}

void Command::part(std::string msg) {
			(void)msg;
			std::cout << "[ PART] ]" << std::endl;
	std::cout << "content: " << msg << "\n";
}

void Command::ping(std::string msg) {
			(void)msg;
			std::cout << "[ PING ]" << std::endl;
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