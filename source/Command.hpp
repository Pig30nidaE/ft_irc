#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <unistd.h>

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


		void sendMsg(User &user, std::string msg) const;

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
		std::string ping(std::string msg, User &user);
		// ----client INVITE---
		void invite(std::string nick, std::string channel, ChannelContainer &cc, User &user);
		// ----topic------------
		std::string topic(Channel &channel, std::string topic);
		// ----mode------------- //LSH
		//void mode(std::string channel, std::string flag, std::string parm);
		void mode(std::vector<std::string> msgs, User& user, ChannelContainer& cc);
		//만약 이 함수 쓰려면 parameter를 user.changeMode와 같이 넘겨야할듯?



	private:
		std::string serverPass; // server password 검사를 위해 생성자를 통해 받아오자
		int	checkLogin(User user);

		// PASS ERROR
		class Err_anyError : public std::exception {
			public:
				Err_anyError(const char *command) : _command(command) {}
				const char *what() const throw() {
					return _command;
				}
			private:
				const char *_command;
		};

		class Err_needmoreparams : public std::exception {
			public:
				const char* what(void) const throw() {
					return ":Not enough parameters\n";
				}
		};

		class ERR_PASSWDMISMATCH : public std::exception {
			public:
				const char* what(void) const throw() {
					return ":Password incorrect\n";
				};
		};
		
		class Err_alreadyregistered : public std::exception {
			public:
				const char* what(void) const throw() {
					return " :You may not reregister\n";
				};
		};
		// PASS ERROR

};


#endif