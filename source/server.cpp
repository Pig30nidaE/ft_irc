#include "./Server.hpp"


Server::Server(void) {
    if ((serverSocket = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        exitWithPerror("socket() error\n" + std::string(strerror(errno)));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(8080);

    // 서버 소켓 바인드
    if (bind(serverSocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        exitWithPerror("bind() error\n" + std::string(strerror(errno)));

    // 서버 소켓 listen 설정
    if (listen(serverSocket, MAXVISITE) == -1)
        exitWithPerror("listen() error\n" + std::string(strerror(errno)));
    // 서버 소켓 NonBlocking 설정
    fcntl(serverSocket, F_SETFL, O_NONBLOCK);
    
    /* init kqueue */
    // kqueue 생성
    if ((kq = kqueue()) == -1)
        exitWithPerror("kqueue() error\n" + std::string(strerror(errno)));

	std::cout << "Server constructor!!\n";
}

void Server::serverStart(void) {

    int newEvents;
    struct kevent* currEvent;
    
    changeEvents(static_cast<uintptr_t>(serverSocket), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    while (1)
    {
        sleep(1);
        newEvents = kevent(kq, &changeList[0], changeList.size(), eventList, 8, NULL);
        if (newEvents == -1)
            exitWithPerror("kevent() error\n" + std::string(strerror(errno)));
        changeList.clear(); // clear changeList for new changes
        for (int i = 0; i < newEvents; ++i)
        {
            currEvent = &eventList[i];
            if (currEvent->flags & EV_ERROR)
                errorEventHandler(currEvent->ident);
            else if (currEvent->filter == EVFILT_READ)
                readEventHandler(currEvent->ident);
        }
    }
    return ;
}

// READ event!!
void Server::readEventHandler(uintptr_t fd) {
    char buf[MAX_MSG];
    
    if (fd == serverSocket)
    {
        struct UserDto user;
        int clientSocket;
        if ((clientSocket = accept(serverSocket, NULL, NULL)) == -1)
            exitWithPerror("accept() error\n" + std::string(strerror(errno)));
        std::cout << "accept new client: " << clientSocket << std::endl; // test
        fcntl(clientSocket, F_SETFL, O_NONBLOCK);
        changeEvents(static_cast<uintptr_t>(clientSocket), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        user.id = clientSocket;
        user.msg = "";
        user.status = 0;
        clients[clientSocket] = user;
    }
    else {
        ssize_t n = recv(fd, buf, MAX_MSG, 0);
        if (n <= 0)
        {
            std::cerr << "client read error!" << std::endl;
            disconnectClient(fd);
        }
        else
        {
            buf[n] = '\0';
            clients[fd].msg = buf;
            std::cout << "recv: " << buf << "\n";
            // go channelContainer
    
        }
    }
}

void Server::errorEventHandler(uintptr_t fd) {
    if (fd == serverSocket)
        exitWithPerror("server socket error");
    else
    {
        std::cerr << "client socket error" << std::endl;
        disconnectClient(fd);
    }
}

Server::~Server() {
    for (std::map<uintptr_t, UserDto>::iterator iter = clients.begin(); iter != clients.end(); iter++) {
        if (close(iter->first) == -1)
            std::cout << "client socket close ERROR\n";
    }
    if (close(kq) == -1)
        std::cout << "Kqueue close ERROR\n";
    if (close(serverSocket) == -1)
        std::cout << "Kqueue close ERROR\n";
	std::cout << "server destroyed!!\n";
}

Server &Server::operator=(const Server &other)
{
    (void)other;
    return *this;
}

Server::Server(const Server &other)
{
    *this = other;
}

void Server::disconnectClient(uintptr_t clientFd)
{
    std::cout << "client disconnected: " << clientFd << std::endl;
    close(clientFd);
    clients.erase(clientFd);
}


void Server::changeEvents(uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
    struct kevent temp_event;

    EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    changeList.push_back(temp_event);
}


void Server::exitWithPerror(const std::string& msg)
{
    std::cerr << msg << std::endl;
    exit(EXIT_FAILURE);
}


