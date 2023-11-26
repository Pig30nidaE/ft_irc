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


		void parser(std::string msg);

	private:
		void pass();
		void nick();
		void user();
		void join();
		void part();
		void ping();
};


#endif