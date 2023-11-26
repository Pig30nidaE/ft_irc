#include "User.hpp"

int main() {
	ChannelContainer Server;
	User test;
	test.joinChannel("test", Server);
	std::cout << test.getJoinedChannel(Server)->getChannelName() << std::endl;
	test.exitChannel(Server);
	std::cout << test.getJoinedChannel(Server)->getChannelName() << std::endl;
}