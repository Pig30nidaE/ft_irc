#include "./Server.hpp"
#include "../func/User.hpp"
#include "../func/ChannelContainer.hpp"
// #include "../func/Channel.hpp"
#include "./Command.hpp"


Server::Server(void) {
    return ;
}

Server::Server(std::string port, std::string pass) : channelContainer(new ChannelContainer()), command(new Command(pass)) {
    if ((serverSocket = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        exitWithPerror("socket() error\n" + std::string(strerror(errno)));

    //test // 초기화 리스트에서 Command()생성자로 서버 패스워드 설정
    // command->setServerPass(pass); //
    // setServerPass(pass);
    // command
    //test

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(std::atoi(port.c_str()));

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
            else if (currEvent->filter == EVFILT_WRITE)
            {
                /* send data to client */
                std::map<uintptr_t, User>::iterator it = clients.find(currEvent->ident);
                if (it != clients.end())
                {
                    User *user = &(it->second);
                    std::vector<std::string> sendBuf = user->getSendBuf();
                    if (sendBuf.size() > 0)
                    {
                        int n;
                        for (std::vector<std::string>::iterator iter = sendBuf.begin(); iter != sendBuf.end(); iter++) {
                            std::cout << "[SERVER]:" << *iter << "\n" << "!!!!!!fd:" << currEvent->ident << "\n";
                            if ((n = write(currEvent->ident, (*iter).c_str(), (*iter).size()) == -1))
                            {
                                std::cerr << "client write error!" << std::endl;
                                disconnectClient(currEvent->ident);  
                            }
                        }
                        user->initSendBuf();
                    }
                }
            }
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
    struct sockaddr_in clientAddr; // test
    socklen_t clientLen; // test

    if ((clientSocket = accept(serverSock,(struct sockaddr*)&clientAddr, &clientLen)) == -1)
        exitWithPerror("accept() error\n" + std::string(strerror(errno)));
    std::cout << "accept new client: " << clientSocket << std::endl; // test
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
        std::cout << "--------------------------\nrecv: " << sBuf << "--------------------------\n";
        // if (isValidString(sBuf))
            parser(sBuf, clients[fd]);
        // else
            // sendMsg(*(clients[fd]), "invalid msg\r\n");
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

void Server::disconnectClient(int sock)
{
    int fd;
    User &user = clients[sock];
    std::vector<User *> &userList = channelContainer->getUserList();
    std::vector<Channel *> &joinedChannelList = user.getJoinedChannelList(*channelContainer);
    fd = sock;

    // 가입된 각 채널의 유저리스트에서 유저 삭제
    for (std::vector<Channel *>::iterator iter = joinedChannelList.begin(); iter != joinedChannelList.end(); iter++){
        (*iter)->userDel(user);
    }
    // 채널컨테이너의 유저리스트에서 유저 삭제
    for (std::vector<User *>::iterator iter = userList.begin(); iter != userList.end();) {
        if ((*iter)->getUserSock() == fd) {
            iter = userList.erase(iter); // erase function returns the next iterator of the deleted element
        } else {
            ++iter; // move to the next element
        }
    }
    // server의 clients에서 user 제거 및 delete
    for (std::map<uintptr_t, User>::iterator iter = clients.begin(); iter != clients.end();) {
        if ((iter->second).getUserSock() == fd) {
            iter = clients.erase(iter); // erase 함수의 반환값을 사용하여 반복자를 갱신
            break; // Stop iterating since it found and deleted the specific element
        } else {
            ++iter; // move to the next element
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


// ------------test용----------
void Server::setServerPass(std::string serverPass) { // test용
    this->serverPass = serverPass;
    command->setServerPass(serverPass);
}

int Server::executeCommand(std::vector<std::string> msgs, User &user) {
	std::string arr[NUMCOMMANDS] = {"PASS", "NICK", "USER", "JOIN", "PART", "PING", "KICK", "INVITE", "TOPIC", "MODE", "QUIT", "PRIVMSG"};
    std::string result;
    Channel     *tmp;
	int         flag = -1;
    int         userPass;

	if (msgs.empty()) { // msgs가 비어있다면
        std::cout << "msgs is empty!!\n";
        return 0;
    }
	for (size_t i = 0; i < NUMCOMMANDS; i++) // command 개수만큼 반복
	{
		//if (msgs[0].compare(0, 4, arr[i]) == 0) 
        if (msgs[0] == arr[i])
			flag = i; // 찾은 command 플래그 저장
	}
    userPass = user.getUserIsPass();
    int status = 0;
    try
    {
        // PASS 마지막만 적용하기 위한 코드
        if ((userPass == 1 || userPass == 2 || userPass == 3) && flag != PASS) { // 마지막으로 남아있는 user의 isPass를 검사하여 pass명령어 진행
            command->checkLastPassState(user);
        }
        // 레지스터 전 다른 명령어 사용에 대한 예외처리
        if (userPass != 5 && !(flag >= -1 && flag <= 2)) {
            throw Command::ErrNotRegistered(arr[flag]);
        }
        // PASS 확인 이전에 다른 명령어에 예외처리
        else if (userPass == 0 && (flag != PASS && flag != -1)) {
            throw Command::ErrNotRegistered(arr[flag]);
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
                    // sendMsg(user, ":" + user.getNickName() + "!root@127.0.0.1 JOIN :#test\n:irc.local 353 " + user.getNickName() + " = #test :@" +user.getNickName()+"\n:irc.local 366 " + user.getNickName() + " #test :End of /NAMES list.\n");
                    // tmp = user.getSpecificChannelJoined(msgs[1], *channelContainer);
                    // broadcastMsg(user, tmp, ":" + user.getNickName() + "!root@127.0.0.1 JOIN :#test\n");
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
       
            case INVITE: //LSH
                command->invite(msgs[1].c_str(), msgs[2].c_str(), *channelContainer, user);
                // 추후 수정
                break;

            case TOPIC:
                command->topic(*channelContainer, msgs, user);
                break;
            case MODE:
                command->mode(msgs, user, *channelContainer);
                break;
            case QUIT:
                std::cout << "QWEHQWHEQHWE" << std::endl;
                break;
            case PRIVMSG:
                command->privmsg(msgs, user, *channelContainer);
                break;

            default:
                if (user.getUserIsPass() == 5) { // 회원가입 전 에러 안 던짐
                    throw Command::ErrUnknownCommand(user.getNickName(), msgs[0]);// msgs[0]을 대문자로 변환해야됨
                } 
                break;
        }
    }
    catch(const std::exception& e) {
        std::cout << "-- [CATCH]: " << flag << " --\n";
        if (user.getUserIsPass() == 2 || user.getUserIsPass() == 3) { // 수정 요함
            std::cout << "disconnect User: " << flag << "\n";
            disconnectClient(user.getUserSock());
            return DISCONNECT;
        }
        std::cerr << "[Catch]: " <<  e.what();
        sendMsg(user, e.what());
        return DISCONNECT;
    }
    if ((flag == USER || flag == NICK) && userPass == 4 && user.getNickName() != "" && user.getUserName() != "") {
        user.setUserIsPass(5); // USER NICK 상관없이 register하는 예외처리
        user.setClient(user.getNickName(), user.getUserName(), user.getHostName());
        sendMsg(user, RPL_WELCOME(user.getNickName(), user.getClient()));
    }
    return 0;
	std::cout << "-----------------------------\n";
}



void Server::parser(std::string msg, User &user) {
    int         cr = 0;
    int         lf = 0;
    int         crlf = 0;
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
    int i = 0;
    std::stringstream iss(msg);
    std::string str;
    std::vector <std::string> msgs;

    while (iss >> str) {
        msgs.push_back(str);
    }
    for (std::vector<std::string>::iterator iter = msgs.begin(); iter != msgs.end(); iter++) { // tester
        std::cout << "[msg]:" << *iter << "/" << (*iter).size() << "\n";
    }
    if (executeCommand(msgs, user) == DISCONNECT)
        return DISCONNECT;
    return 0;
}



void	Server::sendMsg(User &user, const std::string &msg) const {
    if (msg == "")
        return ;
    user.setSendBuf(msg);
	// write(user.getUserSock(), msg.c_str(), msg.length() + 1);
}

void	Server::broadcastMsg(User &user, Channel *channel, const std::string &msg) const {
	std::vector<User *> userList;
    int sock; //test
    std::cout << "[BroadCastMsg]\n";

    if (!channel) {
        std::cout << "channel is NULL\n";
        return ; // err 던지기
    }
	userList = channel->getUserList();
	for (std::vector<User*>::iterator iter = userList.begin(); iter != userList.end(); iter++) {
        sock = (**iter).getUserSock(); // test
        if (sock != user.getUserSock()) {
            std::cout << "[BROAD] send to " << sock << "\n";// test
            sendMsg(**iter, msg);
        }
	}
}


int stringToInt(const std::string& str) {
    int result;

    // std::istringstream 객체를 생성하고 문자열을 넣음
    std::istringstream iss(str);

    // 정수로 변환
    if (!(iss >> result)) {
        // 스트림의 상태 플래그를 확인하여 에러 처리
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

// int Server::isValidChar(char ch) {
//        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') ||
//         ch == '-' || ch == '[' || ch == ']' || ch == '\\' || ch == '`' || ch == '^' ||
//         ch == '{' || ch == '}' || ch == '#' || ch == '$' || ch == ' ' || ch == '\b' ||
//         ch == '\0' || ch == '\r' || ch == '\n' || ch == ',' || ch == '&' || ch == '@' || ch == '!'){
//         return true;
//     } else {
//         return false;
//     } 
// }

// int Server::isValidString(std::string str) {
//     size_t length = str.length();
//     for (size_t i = 0; i < length; i++) {
//         if (!isValidChar(str[i])) {
//             return false;
//         }
//     }
//     return true;
// }