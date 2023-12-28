#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <sstream>
#include <unistd.h>
#include "../NumericReplies.hpp"

#define	NEEDMOREPARMS 2
#define	PASSMISMATCH 3
#define PASSSUCCESS 1
#define CHANNELISFULL 1
#define BADCHANNELKEY 2
#define INVITEONLYCHAN 3
#define JOINCHANNEL 4
#define NEWCHANNEL 5

class User;
class ChannelContainer;
class Channel;

class Command {
	public:
		Command(void);
		~Command();
		Command(Command &other);
		Command operator=(Command &other);



		//--------teset용---------
		Command(std::string serverPass); // test용
		void setServerPass(std::string serverPass); // test용


		void sendMsg(User &user, const std::string &msg) const;
		void broadcastMsg(User &user, Channel *channel, const std::string &msg) const;

		//----server 입장-----
		// PASS
		void pass(std::vector<std::string> msgs, User &user);
		void checkLastPassState(User &user) const;

		// NICK
		int nick(std::vector<std::string> msgs, ChannelContainer cc, User &user);

		// USER
		void user(std::vector<std::string> msgs, User &user);
		
		// ----channel join-----
		int	 join(std::vector<std::string> msgs, ChannelContainer &cc, User &user);
		// ----channel 나감-----
		void part(std::vector<std::string> msgs, ChannelContainer &cc, User &user);
		// kick
		void kick(std::vector<std::string> msgs, ChannelContainer &cc, User &user);
		// ----client PING----
		void ping(std::vector<std::string> msgs, User &user);
		// ----client INVITE---
		void invite(std::string nick, std::string channel, ChannelContainer &cc, User &user);
		// ----topic------------
		std::string topic(ChannelContainer& cc, Channel &channel, std::string topic, User& user);
		// ----mode------------- //LSH
		//void mode(std::string channel, std::string flag, std::string parm);
		void mode(std::vector<std::string> msgs, User& user, ChannelContainer& cc);
		//만약 이 함수 쓰려면 parameter를 user.changeMode와 같이 넘겨야할듯?

		// 공통
		class ErrNotRegistered : public std::invalid_argument {
			public:
				ErrNotRegistered(const std::string& client) : std::invalid_argument(client + " :You have not registered\n") {}
		};

	private:
		std::string serverPass; // server password 검사를 위해 생성자를 통해 받아오자
		int	checkLogin(User user);

		// ---------EXCEPTION--------
		// ----PASS----
		class ErrPasswdMisMatch : public std::exception {
			public:
				const char* what(void) const throw() {
					return " PASS :Password incorrect\n";
				};
		};
		// -----PASS and USER-------
		class ErrAlreadyRegistered : public std::invalid_argument { 
			public:
				ErrAlreadyRegistered(const std::string &command) : std::invalid_argument(command + ":You may not reregister\n") {}
		};

		// NICK ERROR
		class ErrNickNameInUser : public std::invalid_argument {
			public:
				ErrNickNameInUser(const std::string& nickname) : std::invalid_argument(nickname + " :Nickname is already is user\n") {}
		};

		// USER ERROR

		// JOIN ERROR
		class ErrTooManyChannels : public std::invalid_argument {
			public:
				ErrTooManyChannels(const std::string &channelName) : std::invalid_argument(channelName + " :You have joined too many channels\n") {}
		};

		class ErrChannelIsFull : public std::invalid_argument {
			public:
				ErrChannelIsFull(const std::string &channelName) : std::invalid_argument(channelName + " :Cannot join channel (+l)\n") {}
		};

		class ErrBadChannelKey : public std::invalid_argument {
			public:
				ErrBadChannelKey(const std::string &channelName) : std::invalid_argument(channelName + " :Cannot join channel (+k)\n") {}
		};

		class ErrInviteOnlyChan : public std::invalid_argument {
			public:
				ErrInviteOnlyChan(const std::string &channelName) : std::invalid_argument(channelName + " :Cannot join channel (+i)\n") {}
		};

		// 공통
		class ErrNeedMoreParams : public std::invalid_argument {
			public:
				ErrNeedMoreParams(const std::string& command) : std::invalid_argument(command + " :Not enough parameter\n") {}
		};
};

#endif