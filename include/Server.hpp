#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/event.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include <string.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <map>
#include "NumericReplies.hpp"

#define MAXVISITE 		10000
#define PORT 			8080
#define MAX_MSG 		512
#define	NEEDMOREPARMS 	2
#define	PASSMISMATCH 	3
#define PASSSUCCESS 	1
#define DISCONNECT 		1
#define NUMCOMMANDS 	12

enum {
	PASS,
	NICK, 
	USER,
	JOIN, 
	PART,
	PING,
	KICK,
	INVITE,
	TOPIC,
	MODE,
	QUIT,
	PRIVMSG,
};

class User;
class Command;
class Channel;
class ChannelContainer;

class Server {
	private:
		int 						serverSocket;
		struct sockaddr_in 			serverAddr;
		int 						kq;
		std::vector<struct kevent> 	changeList;
		struct kevent 				eventList[8];
		std::map<uintptr_t, User> 	clients;
		ChannelContainer 			*channelContainer;
		Command 					*command;
		std::string 				serverPass;

		int executeCommand(std::vector<std::string> msgs, User &user);
		int splitSpace(const std::string &msg, User &user);

		void readEventHandler(uintptr_t fd);
		void newUser(int serverSock);
		void recvMsg(uintptr_t fd);

		void errorEventHandler(uintptr_t fd);
		
		void disconnectClient(int sock);

		void broadcastMsg(User &user, Channel *channel, const std::string &msg) const;
		void sendMsg(User &user, const std::string &msg) const;

		int  stringToInt(const std::string& str);
		int  isValidChar(char ch);
		int  isValidString(std::string str);

		void changeEvents(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
		void exitWithPerror(const std::string& msg);

		Server &operator=(const Server &other);
		Server(const Server &other);
	public:
		Server(void);
		Server(std::string port, std::string pass);
		Server(int serverPass);
		~Server();

		void serverStart(void);
		void setServerPass(std::string serverPass);
		void parser(std::string msg, User &user);

		ChannelContainer& getChannelContainer(void) {
			return *channelContainer;
		}
};

#endif