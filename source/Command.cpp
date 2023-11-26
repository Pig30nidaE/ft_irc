#include "Command.hpp"
#include "../dong_func/User.hpp"
#include "../dong_func/ChannelContainer.hpp"
#include "../dong_func/Channel.hpp"

void Command::parser(std::string msg, ChannelContainer container) {
	if (msg[0] == 0)
		return ;
	std::cout << "msg: " << msg[0] << ":" << (int)msg[0] << "\n";
	int end = msg.find((char)10);
	if ((&msg[end])[0] != '\0')
		parser(&msg[end + 1]);
}

void Command::pass() {

}

void Command::nick() {

}

void Command::user() {

}

void Command::join() {

}

void Command::part() {

}

void Command::ping() {

}


Command::Command() {}

Command::~Command() {}

Command::Command(Command &other) {
	*this = other;
}
Command Command::operator=(Command &other) {
	(void)other;
	return *this;
}
