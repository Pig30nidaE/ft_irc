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
#include <sstream> // 공백 제거용
#include <iostream>
#include <vector>
#include <unistd.h>
#include <map>

#define MAXVISITE 10000
#define PORT 8080
#define MAX_MSG 512

enum {
	PASS,
	NICK, //O
	USER,
	JOIN, //O
	PART, // channel 인자가 없으면 터짐. 일단 냄겨두겠음.
	PING,
	KICK,
	INVITE,
	TOPIC,
	MODE,
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

		//------------ sanghyol 건듦.
		ChannelContainer& getChannelContainer(void) {
			return *channelContainer;
		}

	private:
		void		executeCommand(std::string msg, User &user);
		int			countSpace(std::string msg);
		void splitSpace(std::string msg);

		void readEventHandler(uintptr_t fd);
		void newUser(int serverSock);
		void recvMsg(uintptr_t fd);

		void errorEventHandler(uintptr_t fd);
		
		void disconnectClient(User *user);

		void broadcastMsg(User user, Channel *channel, std::string msg) const;
		void sendMsg(User user, std::string msg) const;

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