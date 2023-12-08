#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
class User;
class ChannelContainer;
// class Channel;

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

		void pass(std::string msg);
	private:
		std::string serverPass; // server password 검사를 위해 생성자를 통해 받아오자
		//----server 입장-----
		void nick(std::string msg, ChannelContainer cc, User &user);
		void user(std::string msg, User &user);

		// ----channel join-----
		void join(std::string msg, ChannelContainer &cc, User &user);

		// ----channel 나감-----
		void part(std::string msg, ChannelContainer &cc, User &user);

		// ----client PING----
		void ping(std::string msg);

		// kick
		void kick(std::string msg);
};


#endif