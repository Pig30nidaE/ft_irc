#include "./Server.hpp"
#include "../func/User.hpp"
#include "../func/ChannelContainer.hpp"
// #include "../func/Channel.hpp"
#include "./Command.hpp"


Server::Server(void) : channelContainer(new ChannelContainer()), command(new Command()) {
    if ((serverSocket = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        exitWithPerror("socket() error\n" + std::string(strerror(errno)));

    //test
    setServerPass("123");
    //test
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(8080);

    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    // 서버 소켓 바인드
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
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
        // sleep(1);
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
    if (fd == static_cast<uintptr_t> (serverSocket)) {
        newUser(static_cast<int>(fd));
    }
    else if (clients.find(fd) != clients.end()) {
        recvMsg(fd);
    }
}

void Server::newUser(int serverSock) {
    int clientSocket;
    User *user;
    struct sockaddr_in clientAddr; // test
    socklen_t clientLen; // test

    if ((clientSocket = accept(serverSock,(struct sockaddr*)&clientAddr, &clientLen)) == -1)
        exitWithPerror("accept() error\n" + std::string(strerror(errno)));
    std::cout << "accept new client: " << clientSocket << std::endl; // test
    fcntl(clientSocket, F_SETFL, O_NONBLOCK);
    changeEvents(static_cast<uintptr_t>(clientSocket), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    
    user = new User();
    user->setUserSock(clientSocket);
    clients[clientSocket] = user;
    getChannelContainer().getUserList().push_back(user); // channel_container의 user_list에도 추가해야해서 넣음. //LSH
}

void Server::recvMsg(uintptr_t fd) {
    char buf[MAX_MSG];
    ssize_t n = recv(static_cast<int>(fd), buf, MAX_MSG, 0);

    if (n <= 0)
    {
        std::cerr << "client read error!" << std::endl;
        disconnectClient(clients[fd]);
    }
    else
    {
        // std::cout << "n: " << n << "\n";
        // std::cout << "buf[n]: " << buf[n] << ", " << (int)buf[n]<< "\n";
        // std::cout << "buf[n - 1]: " << buf[n - 1] << ", " << (int)buf[n - 1]<< "\n";
        // if (n >= 2) {
        //     std::cout << "buf[n - 2]: " << buf[n - 2] << ", " << (int)buf[n - 2]<< "\n";
        // }
        buf[n] = '\0';
        std::string test = buf;
        std::cout << "--------------------------\nrecv: " << test << "--------------------------\n";
        //parser(test, *clients[fd]);
        executeCommand(test, *clients[fd]);
        std::cout << "NICK: " << (*clients[fd]).getNickName() << std::endl;

    }
}


void Server::errorEventHandler(uintptr_t fd) {
    if (fd == static_cast<uintptr_t>(serverSocket))
        exitWithPerror("server socket error");
    else
    {
        std::cerr << "client socket error" << std::endl;
        disconnectClient(clients[fd]);
    }
}

Server::~Server() {
    for (std::map<uintptr_t, User*>::iterator iter = clients.begin(); iter != clients.end(); iter++) {
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

void Server::disconnectClient(User *user)
{
    int fd;

    fd = user->getUserSock();
    std::cout << "client disconnected: " << fd << std::endl;
    close(fd);
    // clients.erase(fd);
    delete clients[fd]; // User delete
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


// ------------test용----------
void Server::setServerPass(std::string serverPass) { // test용
    this->serverPass = serverPass;
    command->setServerPass(serverPass);
}

void Server::executeCommand(std::string msg, User &user) {
	std::string	arr[10] = {"PASS", "NICK", "USER", "JOIN", "PART", "PING", "KICK", "INVITE", "TOPIC", "MODE"};
	std::string content;
    Channel *tmp;
	int			flag;
	int			end;

	if (msg[0] == 0)
		return ;
	//------------------------------------
    flag = -1;
	// std::cout << "msg: " << msg[0] << ":" << (int)msg[0] << "\n";
	for (size_t i = 0; i < 10; i++)
	{
		if (msg.compare(0, 4, arr[i]) == 0) 
			flag = i;
	}

	end = msg.find((char)10);
	content = msg.substr(5, end - 5);
	switch (flag)
	{
		case PASS:
            try
            {
                command->pass(content);
                user.setUserIsPass(1);
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            
			break;
		
		case NICK:
            command->nick(content, *channelContainer, user);
			break;

		case USER:
            sendMsg(user, command->user(content, user));
			break;
			
		case JOIN:
            command->join(content, *channelContainer, user);
            sendMsg(user, ":nickname!root@127.0.0.1 JOIN :#test\n:irc.local 353 nickname = #test :@nickname\n:irc.local 366 nickname #test :End of /NAMES list.\n");
            tmp = user.getSpecificChannelJoined(content, *channelContainer);
            broadcastMsg(user, tmp, ":nickname_!root@127.0.0.1 JOIN :#test\n");
			break;
			
		case PART:
            command->part(content, *channelContainer, user);
			break;
			
		case PING:
            command->ping(content, user);
			break;

        case KICK:
            command->kick(content, *channelContainer, user);
            break;
        
        //case INVITE: //LSH
        //    command->invite() // invite 맞춰서 수정해야함.
        //    // 추후 수정
        //    break;

        case TOPIC:
            tmp = user.getSpecificChannelJoined(content, *channelContainer);
            sendMsg(user, command->topic(*tmp, content)); // 임시
            break;

        //case MODE:
        //    command->mode(content, content, content); // LSH 이거 그냥 user.changeMode로 인자넘기는게 나을거같은디
        //    break;
        
		default:
			std::cout << "[ COMMAND ERROR!!!!!!!!!!!!!! ]\n";
			break;
	}
	//--------------------------------------
	std::cout << "-----------------------------\n";
	if ((&msg[end])[0] != '\0')
		parser(&msg[end + 1], user);

}



void Server::parser(std::string msg, User &user) {
	int			end;
    int         cr = 0;
    int         lf = 0;
    int         crlf = 0;
    int         lfcr = 0;
    int          i = 0;
    std::string temp;

    // std::cout << "\\r : " << (int)'\r' << "\n";
    // std::cout << "\\n : " << (int)'\n' << "\n";
    // std::cout << "msg:" << msg << "\n\n";
    // cr = msg.find('\r', cr);
    // std::cout << "cr을 찾은것:" << &msg[cr] << "| len: " << strlen(&msg[cr]) << "\n";
    // lf = msg.find('\n', lf);
    // std::cout << "lf를 찾은것:" << &msg[lf] << "| len: " << strlen(&msg[lf]) << "\n";
    // crlf = msg.find("\r\n", crlf);
    // if (crlf == std::string::npos) {
    //     std::cout << "마지막줄:" << msg << "\n";
    // }
    // std::cout << "crlf를 찾은것:" << &msg[crlf] << "| len: " << strlen(&msg[crlf]) << "\n";
    // lfcr = msg.find("\n\r", lfcr);
    // std::cout << "lfcr를 찾은것:" << &msg[lfcr] << "| len: " << strlen(&msg[lfcr]) << "\n";

    if ((cr = msg.find('\r')) == std::string::npos) {
        if ((lf = msg.find('\n', lf)) == std::string::npos) {
            temp = user.getBuf();
            std::cout << "befor: " << temp << "\n";
            temp += msg;
            std::cout << "after: " << temp << "\n";
            user.setBuf(temp);            
        }
        else {
            temp = user.getBuf();
            std::cout << "befor: " << temp << "\n";
            temp += msg;
            std::cout << "after: " << temp << "\n";
            splitSpace(temp);
        }
    }
    else if ((crlf = msg.find("\r\n", crlf)) != std::string::npos) {
        splitSpace(msg.substr(0, crlf));
        if ((crlf = msg.find("\r\n", crlf)) != std::string::npos) {
            parser(&msg[crlf + 1], user);
        }
    }
	// end = msg.find((char)10);
	// content = msg.substr(5, end - 5);
        
	// if ((&msg[end])[0] != '\0')
	// 	parser(&msg[end + 1], user);
}

void    Server::splitSpace(std::string msg) {
    int i = 0;
    std::stringstream iss(msg);
    std::string str;
    std::vector <std::string> msgs;

    while (iss >> str) {
        msgs.push_back(str);
    }
    for (std::vector<std::string>::iterator iter = msgs.begin(); iter != msgs.end(); iter++) {
        std::cout << "msg:" << *iter << "\n";
    }
    // return msgs;
}


void	Server::sendMsg(User user, std::string msg) const {
    if (msg == "")
        return ;
	write(user.getUserSock(), msg.c_str(), msg.length() + 1);
}

void	Server::broadcastMsg(User user, Channel *channel, std::string msg) const {
	std::vector<User *> userList;

    if (channel == nullptr)
        return ; // err 던지기
	userList = channel->getUserList();
	for (std::vector<User*>::iterator iter = userList.begin(); iter != userList.end(); iter++) {
        if ((**iter).getUserSock() != user.getUserSock())
            sendMsg(**iter, msg);
	}
}
