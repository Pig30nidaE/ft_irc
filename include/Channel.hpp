#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>

#include "User.hpp"

#define MODE_CHECK_TYPE(type1, type2) ((type1) & (type2))

#define ALL_MODES_OFF      		0x00
#define MODE_INVITE_ONLY   		0x01
#define MODE_TOPIC_LIMIT   		0x02
#define MODE_CHANNEL_KEY   		0x04
#define MODE_OPERATOR_GRANTED 	0x08
#define MODE_CAPACITY_LIMIT 	0x10

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
		void 		setTopic(std::string const toSetTopic) {
			channelTopic = toSetTopic;
		};
};

class Mode {
	private:
		int	type;
	public:
		Mode(int type):type(type) {};
		~Mode() {};

		int	 getType(void) const {
			return type;
		};
		bool checkType(int const toCheck) {
			return (toCheck & type);
		};
		void turnOnType(int const toTurnOn) {
			type |= toTurnOn;
		};
		void turnOffType(int const toTurnOff) {
			type &= ~(toTurnOff);
		};
};

class Channel {
	private:
		std::string 				channelName;
		std::vector<User*>			userList;
		std::vector<int>			invitedUserList;
		std::vector<int>			channelOpList;
		std::string					password;
		int							capacity;
		Topic						*topic;
		Mode						*mode;
	public:
		Channel(std::string name);
		~Channel();

		void						userAdd(User& user);
		void						userDel(User& user);
		void						operatorAdd(User& user);
		bool						isOperator(User& user);
		bool					 	isOnTheChan(User& user);
		int							isClientOpList(int sock);

		std::string					getChannelName(void) const;
		void						setChannelName(std::string const channelname);
		std::vector<User*>	 		&getUserList(void);
		User*						getSpecficUser(std::string nickname);
		std::vector<int>			&getInvitedUserList(void);
		std::string					getPassword(void) const;
		void						setPassword(std::string pwd);
		Topic						&getTopicRef(void);
		Mode						&getModeRef(void);
		int							getCapacity(void);
		void						setCapacity(int size);
		std::vector<int>			&getChannelOpList(void);
};

#endif