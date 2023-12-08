#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/event.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
//----------------------

//----------------------
#include <string.h>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <map>

#define MAXVISITE 10000
#define PORT 8080
#define MAX_MSG 512

enum {
	PASS,
	NICK,
	USER,
	JOIN,
	PART,
	PING
};

class User;
class Command;
class Channel;
class ChannelContainer;

class Server {
	public:
		Server(void);
		Server(int serverPass);
		~Server();
		void serverStart(void);

		//---test용---
		void setServerPass(std::string serverPass); // test 용
		void parser(std::string msg, User &user);
		//------------

	private:
		void readEventHandler(uintptr_t fd);
		void newUser(int serverSock);
		void recvMsg(uintptr_t fd);

		void errorEventHandler(uintptr_t fd);
		
		void disconnectClient(User *user);

		// --------utils-------
		void changeEvents(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
		void exitWithPerror(const std::string& msg);
		Server &operator=(const Server &other);
		Server(const Server &other);

		int serverSocket;
		struct sockaddr_in serverAddr;
		int kq;
		std::vector<struct kevent> changeList;
		struct kevent eventList[8];
		std::map<uintptr_t, User*> clients; // User 목록
		ChannelContainer *channelContainer;
		Command *command;
		std::string serverPass;
};

#endif