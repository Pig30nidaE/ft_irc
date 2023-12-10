#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include "User.hpp"

#define MODE_CHECK_TYPE(type1, type2) ((type1) & (type2))

#define ALL_MODES_OFF      		0x00  // off
#define MODE_INVITE_ONLY   		0x01  // i
#define MODE_TOPIC_LIMIT   		0x02  // t
#define MODE_CHANNEL_KEY   		0x04  // k
#define MODE_OPERATOR_GRANTED 	0x08  // o
#define MODE_CAPCITY_LIMIT 		0x16  // l

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
		void turnOnType(int const toTurnOn) {
			type |= toTurnOn;
		};
		void turnOffType(int const toTurnOff) {
			type &= ~(toTurnOff);
		};
		bool checkType(int const toCheck) {
			return (toCheck & type);
		};
};

class Channel {
	private:
		int							index;
		std::string 				channelName;
		std::vector<User*>			userList;
		std::vector<int>			bannedUserList;
		std::string					password;
		int							capcity;
		Topic						*topic;
		Mode						*mode;
	public:
		Channel(std::string name);
		~Channel();

		void	userAdd(User& user);
		void	userDel(User& user);
		bool	isUserBan(std::string user_name);

		std::string					getChannelName(void) const;
		void						setChannelName(std::string const channelname);
		std::vector<User*>	 		&getUserList(void);
		std::vector<int>			&getBannedUserList(void);
		std::string					getPassword(void) const;
		void						setPassword(std::string pwd);
		Topic						&getTopicRef(void);
		Mode						&getModeRef(void);
		int							getIndex(void);
		void						setIndex(int idx);
		int							getCapcity(void);
		void						setCapcity(int size);


};

#endif