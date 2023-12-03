#include "User.hpp"

int main() {
	ChannelContainer Server;
	User test(Server);
	test.joinChannel("test", Server);
	test.setIsOperator(true);
	User guest(Server);
	typedef std::vector<Channel*>::iterator ChannelIterator;
	std::vector<Channel*>& joinedChannels = test.getJoinedChannelList(Server);

	for (ChannelIterator ch_iter = joinedChannels.begin(); ch_iter != joinedChannels.end(); ++ch_iter) {
		std::cout << "name : " << (*ch_iter)->getChannelName() << std::endl;
		std::cout << "topic: " << (*ch_iter)->getTopicRef().getTopic() << std::endl;
	}

	test.inviteUser(guest, *test.getSpecificChannelJoined("test", Server), Server);

	typedef std::vector<User*>::iterator UserIterator;
	std::vector<User*>& user_list_in_ch = test.getSpecificChannelJoined("test", Server)->getUserList();
	for (UserIterator user_iter = user_list_in_ch.begin();user_iter != user_list_in_ch.end();user_iter++) {
		std::cout << "--------------------" << std::endl;
		std::cout << "user:nick" << std::endl;
		std::cout << (*user_iter)->getUserName() <<\
			":" << (*user_iter)->getSpcfNickName("test") << std::endl;
	}
	User guest2(Server);
	guest.inviteUser(guest2, *test.getSpecificChannelJoined("test", Server), Server);
	test.kickUser(guest, *test.getSpecificChannelJoined("test", Server), Server);
	for (UserIterator user_iter = user_list_in_ch.begin();user_iter != user_list_in_ch.end();user_iter++) {
		std::cout << "--------------------" << std::endl;
		std::cout << "user     |nick" << std::endl;
		std::cout << (*user_iter)->getUserName() <<\
			"|" << (*user_iter)->getSpcfNickName("test") << std::endl;
	}
	test.exitChannel("test", Server);
	if (Server.getChannelList().empty())
		std::cout << "NO CHANNEL" << std::endl;
	//std::cout << test.getJoinedChannel(Server)->getChannelName() << std::endl;
}