#include "User.hpp"

int main() {
	ChannelContainer Server;
	User test;
	test.joinChannel("test", Server);
	typedef std::vector<Channel*>::iterator ChannelIterator;

	std::vector<Channel*>& joinedChannels = test.getJoinedChannelList(Server);
	for (ChannelIterator ch_iter = joinedChannels.begin(); ch_iter != joinedChannels.end(); ++ch_iter) {
		std::cout << "name : " << (*ch_iter)->getChannelName() << std::endl;
		std::cout << "topic: " << (*ch_iter)->getTopicRef().getTopic() << std::endl;
	}

	test.exitChannel("test", Server);
	for (ChannelIterator ch_iter = test.getJoinedChannelList(Server).begin();ch_iter != test.getJoinedChannelList(Server).end();ch_iter++)
		std::cout << (*ch_iter)->getChannelName() << std::endl;
	//std::cout << test.getJoinedChannel(Server)->getChannelName() << std::endl;
}