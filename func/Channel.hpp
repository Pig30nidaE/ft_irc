#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <vector>

#include "User.hpp"

class Topic {
	private:
		std::string channelTopic;
	public:
		Topic(std::string toSetTopic);
		~Topic();
		std::string	getTopic(void) const {
			return channelTopic;
		};
		void setTopic(std::string const toSetTopic) {
			channelTopic = toSetTopic;
		};
};

class Mode {
	private:
		int	type;
	public:
		int	getType(void) const {
			return type;
		};
		void setType(int const toChangeType) {
			type = toChangeType;
		};
};

class Channel {
	private:
		int					index;
		std::string 		channelName;
		std::vector<User&>	userList;
		Topic				topic;
		Mode				mode;
	public:
		Channel(std::string name);
		~Channel();

		void	userAdd(const User& user);
		void	userDel(const User& user);

		std::string			getChannelName(void) const;
		void				setChannelName(std::string const channelname);
		std::vector<User&> 	getUserList(void) const;

};

#endif