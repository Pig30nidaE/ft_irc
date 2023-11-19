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

class Clients;

class Server {
	public:
	Server(void);
	~Server();
	Server &operator=(const Server &other);
	Server(const Server &other);

	void serverStart(void);
	void changeEvents(std::vector<struct kevent>& change_list, uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
	void exitWithPerror(const std::string& msg);
	void disconnectClient(int client_fd, std::map<int, std::string>& clients);
	
	private:
	std::vector<struct kevent> change_list;
	struct kevent event_list[8];
	std::map<int, std::string> clients;
	uintptr_t server_socket;
	int kq;

};




#endif
