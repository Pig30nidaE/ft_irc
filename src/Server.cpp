#include "../include/Server.hpp"
#include "../include/User.hpp"
#include "../include/ChannelContainer.hpp"
#include "../include/Command.hpp"


Server::Server(void) {
    return ;
}

Server::Server(std::string port, std::string pass) : channelContainer(new ChannelContainer()), command(new Command(pass)) {
    if ((serverSocket = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        exitWithPerror("socket() error\n" + std::string(strerror(errno)));
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(std::atoi(port.c_str()));

    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
        exitWithPerror("bind() error\n" + std::string(strerror(errno)));
    if (listen(serverSocket, MAXVISITE) == -1)
        exitWithPerror("listen() error\n" + std::string(strerror(errno)));
    fcntl(serverSocket, F_SETFL, O_NONBLOCK);

    if ((kq = kqueue()) == -1)
        exitWithPerror("kqueue() error\n" + std::string(strerror(errno)));
}

void Server::serverStart(void) {
    int newEvents;
    struct kevent* currEvent;
    
    changeEvents(static_cast<uintptr_t>(serverSocket), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    while (1)
    {
        newEvents = kevent(kq, &changeList[0], changeList.size(), eventList, 8, NULL);
        if (newEvents == -1)
            exitWithPerror("kevent() error\n" + std::string(strerror(errno)));
        changeList.clear();
        for (int i = 0; i < newEvents; ++i)
        {
            currEvent = &eventList[i];
            if (currEvent->flags & EV_ERROR)
                errorEventHandler(currEvent->ident);
            else if (currEvent->filter == EVFILT_READ)
                readEventHandler(currEvent->ident);
            else if (currEvent->filter == EVFILT_WRITE)
            {
                std::map<uintptr_t, User>::iterator it = clients.find(currEvent->ident);
                if (it != clients.end())
                {
                    User *user = &(it->second);
                    std::vector<std::string> sendBuf = user->getSendBuf();
                    if (sendBuf.size() > 0)
                    {
                        int n;
                        for (std::vector<std::string>::iterator iter = sendBuf.begin(); iter != sendBuf.end(); iter++) {
                            if ((n = write(currEvent->ident, (*iter).c_str(), (*iter).size()) == -1))
                            {
                                std::cerr << "client write error!" << std::endl;
                                disconnectClient(currEvent->ident);  
                            }
                        }
                        user->sendBufClear();
                    }
                }
            }
        }
    }
    return ;
}

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
    struct sockaddr_in clientAddr;
    socklen_t clientLen;

    if ((clientSocket = accept(serverSock,(struct sockaddr*)&clientAddr, &clientLen)) == -1)
        exitWithPerror("accept() error\n" + std::string(strerror(errno)));
    fcntl(clientSocket, F_SETFL, O_NONBLOCK);
    changeEvents(static_cast<uintptr_t>(clientSocket), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    changeEvents(static_cast<uintptr_t>(clientSocket), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
    
    clients[clientSocket];
    clients[clientSocket].setUserSock(clientSocket);
    getChannelContainer().getUserList().push_back(&(clients[clientSocket])); // channel_container의 user_list에도 추가해야해서 넣음. //LSH
}

void Server::recvMsg(uintptr_t fd) {
    char buf[MAX_MSG];
    std::string sBuf;
    ssize_t n;


    memset(buf, 0, MAX_MSG);
    n = recv(static_cast<int>(fd), buf, MAX_MSG, 0);
    if (n <= 0)
    {
        sendMsg(clients[fd], "read Error\r\n");
        disconnectClient(fd);
    }
    else
    {
        buf[n] = '\0';
        sBuf.clear();
        sBuf = buf;
        parser(sBuf, clients[fd]);
    }
}

void Server::errorEventHandler(uintptr_t fd) {
    if (fd == static_cast<uintptr_t>(serverSocket))
        exitWithPerror("server socket error");
    else
    {
        std::cerr << "client socket error" << std::endl;
        disconnectClient(fd);
    }
}

Server::~Server() {
    for (std::map<uintptr_t, User>::iterator iter = clients.begin(); iter != clients.end(); iter++) {
        if (close(iter->first) == -1)
            std::cout << "client socket close ERROR\n";
    }
    if (close(kq) == -1)
        std::cout << "Kqueue close ERROR\n";
    if (close(serverSocket) == -1)
        std::cout << "Kqueue close ERROR\n";
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

void Server::disconnectClient(int sock)
{
    int fd;
    User &user = clients[sock];
    std::vector<User *> &userList = channelContainer->getUserList();
    std::vector<Channel *> &joinedChannelList = user.getJoinedChannelList();
    fd = sock;

    for (std::vector<Channel *>::iterator iter = joinedChannelList.begin(); iter != joinedChannelList.end(); iter++){
        (*iter)->userDel(user);
    }
    for (std::vector<User *>::iterator iter = userList.begin(); iter != userList.end();) {
        if ((*iter)->getUserSock() == fd) {
            iter = userList.erase(iter);
        } else {
            ++iter;
        }
    }
    for (std::map<uintptr_t, User>::iterator iter = clients.begin(); iter != clients.end();) {
        if ((iter->second).getUserSock() == fd) {
            iter = clients.erase(iter);
            break;
        } else {
            ++iter;
        }
    }
    close(fd);
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


void Server::setServerPass(std::string serverPass) {
    this->serverPass = serverPass;
    command->setServerPass(serverPass);
}

int Server::executeCommand(std::vector<std::string> msgs, User &user) {
	std::string arr[NUMCOMMANDS] = {"PASS", "NICK", "USER", "JOIN", "PART", "PING", "KICK", "INVITE", "TOPIC", "MODE", "QUIT", "PRIVMSG"};
    std::string result;
	int         flag = -1;
    int         userPass;

	if (msgs.empty()) {
        return 0;
    }
	for (size_t i = 0; i < NUMCOMMANDS; i++)
	{
        if (msgs[0] == arr[i])
			flag = i;
	}
    userPass = user.getUserIsPass();
    try
    {
        if ((userPass == 1 || userPass == 2 || userPass == 3) && flag != PASS) {
            command->checkLastPassState(user);
        }
        if (flag != QUIT) {
            if (userPass != 5 && !((flag >= -1 && flag <= 2))) {
                throw Command::ErrNotRegistered(arr[flag]);
            }
            else if (userPass == 0 && (flag != PASS && flag != -1)) {
                throw Command::ErrNotRegistered(arr[flag]);
            }
        }
        switch (flag)
        {
            case PASS:
                command->pass(msgs, user);
                break;
		
            case NICK:
                command->nick(msgs, *channelContainer, user);
                break;

            case USER:
                command->user(msgs, user);
                break;
			
            case JOIN:
                command->join(msgs, *channelContainer, user);
                break;
            case PART:
                command->part(msgs, *channelContainer, user);
                break;
            case PING:
                command->ping(msgs, user);
                break;
            case KICK:
                command->kick(msgs, *channelContainer, user);
                break;
            case INVITE:
                command->invite(msgs[1].c_str(), msgs[2].c_str(), *channelContainer, user);
                break;
            case TOPIC:
                command->topic(msgs, user);
                break;
            case MODE:
                command->mode(msgs, user);
                break;
            case QUIT:
                command->quit(msgs, user);
                disconnectClient(user.getUserSock());
                return DISCONNECT;
                break;
            case PRIVMSG:
                command->privmsg(msgs, user, *channelContainer);
                break;

            default:
                if (user.getUserIsPass() == 5) {
                    throw Command::ErrUnknownCommand(user.getNickName(), msgs[0]);
                } 
                break;
        }
    }
    catch(const std::exception& e) {
        if (user.getUserIsPass() == 2 || user.getUserIsPass() == 3) {
            disconnectClient(user.getUserSock());
            return DISCONNECT;
        }
        sendMsg(user, e.what());
        return DISCONNECT;
    }
    if ((flag == USER || flag == NICK) && userPass == 4 && user.getNickName() != "" && user.getUserName() != "") {
        user.setUserIsPass(5);
        user.setClient(user.getNickName(), user.getUserName(), user.getHostName());
        sendMsg(user, RPL_WELCOME(user.getNickName(), user.getClient()));
    }
    return 0;
}

void Server::parser(std::string msg, User &user) {
    size_t      cr = 0;
    size_t      lf = 0;
    size_t      crlf = 0;
    std::string temp;

    if (msg == "")
        return ;
    if ((cr = msg.find('\r')) == std::string::npos) {
        if ((lf = msg.find('\n', lf)) == std::string::npos) {
            temp = user.getBuf();
            temp += msg;
            user.setBuf(temp);            
        }
        else {
            temp = user.getBuf();
            temp += msg;
            splitSpace(temp, user);
            user.setBuf("");
        }
    }
    else if ((crlf = msg.find("\r\n", crlf)) != std::string::npos) {
        if (splitSpace(msg.substr(0, crlf), user) == DISCONNECT)
            return ;
        if ((crlf = msg.find("\r\n", crlf)) != std::string::npos) {
            parser(&msg[crlf + 2], user);
        }
    }
}

int    Server::splitSpace(const std::string &msg, User &user) {
    std::stringstream iss(msg);
    std::string str;
    std::vector <std::string> msgs;

    while (iss >> str) {
        msgs.push_back(str);
    }
    if (executeCommand(msgs, user) == DISCONNECT)
        return DISCONNECT;
    return 0;
}



void	Server::sendMsg(User &user, const std::string &msg) const {
    if (msg == "")
        return ;
    user.setSendBuf(msg);
}

void	Server::broadcastMsg(User &user, Channel *channel, const std::string &msg) const {
	std::vector<User *> userList;
    int sock;

    if (!channel) {
        return ;
    }
	userList = channel->getUserList();
	for (std::vector<User*>::iterator iter = userList.begin(); iter != userList.end(); iter++) {
        sock = (**iter).getUserSock();
        if (sock != user.getUserSock()) {
            sendMsg(**iter, msg);
        }
	}
}


int stringToInt(const std::string& str) {
    int result;

    std::istringstream iss(str);

    if (!(iss >> result)) {
        if (iss.eof()) {
            throw std::runtime_error("Error: Empty string");
        } else if (iss.fail()) {
            throw std::runtime_error("Error: Invalid conversion");
        } else {
            throw std::runtime_error("Error converting string to int");
        }
    }

    return result;
}