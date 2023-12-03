#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include "User.hpp"

class User;

class Topic {
	private:
		std::string channelTopic;
	public:
		Topic(std::string toSetTopic):channelTopic(toSetTopic) {};
		~Topic() {};
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
		Mode(int type):type(type) {};
		~Mode() {};
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
		std::vector<User*>	userList;
		Topic				*topic;
		Mode				*mode;
	public:
		Channel(std::string name, std::string topic_contents, int type);
		~Channel();

		void	userAdd(User& user);
		void	userDel(User& user);

		std::string				getChannelName(void) const;
		void					setChannelName(std::string const channelname);
		std::vector<User*>	 	&getUserList(void);
		Topic					&getTopicRef(void);
		Mode					&getModeRef(void);
		int						getIndex(void);
		void					setIndex(int idx);


};

#endif