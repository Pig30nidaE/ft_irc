#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>

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

		// Pass error start
		class ERR_NEEDMOREPARAMS : public std::exception {
			public:
				const char* what(void) const throw() {
					return "ERR_NEEDMOREPARAMS!!\n";
				};
		};
		class ERR_PASSWDMISMATCH : public std::exception {
			public:
				const char* what(void) const throw() {
					return "ERR_PASSWDMISMATCH!!\n";
				};
		};
		// Pass error end
		//----server 입장-----
		void pass(std::string msg);
		void nick(std::string msg, ChannelContainer cc, User &user);
		std::string user(std::string msg, User &user);
		// ----channel join-----
		void join(std::string msg, ChannelContainer &cc, User &user);
		// ----channel 나감-----
		void part(std::string msg, ChannelContainer &cc, User &user);
		// kick
		void kick(std::string msg, ChannelContainer &cc, User &user);
		// ----client PING----
		std::string ping(std::string msg, User &user);
		// ----client INVITE---
		void invite(std::string nick, std::string channel, ChannelContainer &cc, User &user);
		// ----topic------------
		std::string topic(Channel &channel, std::string topic);
		// ----mode------------- //LSH
		//void mode(std::string channel, std::string flag, std::string parm);
		void mode(std::vector<std::string> msg, User& user, ChannelContainer& cc);
		//만약 이 함수 쓰려면 parameter를 user.changeMode와 같이 넘겨야할듯?



	private:
		std::string serverPass; // server password 검사를 위해 생성자를 통해 받아오자

		int	checkLogin(User user);
};


#endif