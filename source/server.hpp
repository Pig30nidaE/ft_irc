#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/event.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>


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
	Server &operator=(const Server &other);
	Server(const Server &other);

	void serverStart(void);
	void changeEvents(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
	void exitWithPerror(const std::string& msg);
	void disconnectClient(uintptr_t clientFd);

	void readEventHandler(uintptr_t fd);
	void errorEventHandler(uintptr_t fd);
	
	private:
	std::vector<struct kevent> changeList;
	struct kevent eventList[8];
	// 소멸자 생성시 close()하기 위해서 clients 관리용 변수가 필요함
	std::map<uintptr_t, UserDto> clients;
	uintptr_t serverSocket;
	int kq;
	struct sockaddr_in server_addr;
};




#endif
