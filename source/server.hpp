#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/event.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include "../dong_func/User.hpp"

//----------------------
#include <string.h>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <map>

#define MAXVISITE 10000
#define PORT 8080
#define MAX_MSG 512

struct UserDto {
	uintptr_t id;
	int status;
	std::string msg;
};

class Server {
	public:
	Server(void);
	~Server();
	void serverStart(void);

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

	std::vector<struct kevent> changeList;
	struct kevent eventList[8];
	std::map<uintptr_t, User*> clients; // User 목록
	int serverSocket;
	int kq;
	struct sockaddr_in serverAddr;
};

#endif