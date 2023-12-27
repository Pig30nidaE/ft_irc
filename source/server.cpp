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
        parser(test, *clients[fd]);
        //executeCommand(test, *clients[fd]);
        //std::cout << "NICK: " << (*clients[fd]).getNickName() << std::endl;

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
    std::vector<User *> &userList = channelContainer->getUserList();

    fd = user->getUserSock();
    std::cout << "client disconnected: " << fd << std::endl;
    // channelContainer의 userList에서 해당 요소 삭제
    std::vector<User*>::iterator iter = userList.begin();
    while (iter != userList.end()) {
        if (*iter == user) {
            iter = userList.erase(iter); // erase 함수는 삭제된 요소의 다음 반복자를 반환
        } else {
            ++iter;
        }
    }
    // -end-
    close(fd);
    delete clients[fd]; // User delete
    clients.erase(fd);
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

void Server::executeCommand(std::vector<std::string> msgs, User &user) {
	std::string arr[10] = {"PASS", "NICK", "USER", "JOIN", "PART", "PING", "KICK", "INVITE", "TOPIC", "MODE"};
    std::string result;
    Channel     *tmp;
	int         flag = -1;
    int         userPass;

	if (msgs.empty()) { // msgs가 비어있다면
        std::cout << "msgs is empty!!\n";
        return ;
    }

	for (size_t i = 0; i < 10; i++) // command 개수만큼 반복
	{
		//if (msgs[0].compare(0, 4, arr[i]) == 0) 
        if (msgs[0] == arr[i])
			flag = i; // 찾은 command 플래그 저장
	}
    userPass = user.getUserIsPass();
    try
    {
        // PASS 마지막만 적용하기 위한 코드
        if ((userPass == 1 || userPass == 2 || userPass == 3) && flag != PASS) { // 마지막으로 남아있는 user의 isPass를 검사하여 pass명령어 진행
            command->checkLastPassState(user);
        }
        // 레지스터 전 다른 명령어 사용에 대한 예외처리
        else if (userPass != 5 && !(flag >= -1 && flag <= 2)) {
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
                if (command->join(msgs, *channelContainer, user) >= 0) {
                    sendMsg(user, ":nickname!root@127.0.0.1 JOIN :#test\n:irc.local 353 nickname = #test :@nickname\n:irc.local 366 nickname #test :End of /NAMES list.\n");
                    tmp = user.getSpecificChannelJoined(msgs[0], *channelContainer);
                    broadcastMsg(user, tmp, ":nickname_!root@127.0.0.1 JOIN :#test\n");
                }
                break;
			
            case PART:
                command->part(msgs, *channelContainer, user);
                break;
			
            case PING:
                command->ping(msgs[0], user);
                break;

            case KICK:
                command->kick(msgs, *channelContainer, user);
                break;
        
            //case INVITE: //LSH
            //    command->invite() // invite 맞춰서 수정해야함.
            //    // 추후 수정
            //    break;

            case TOPIC:
                if (msgs.size() < 2) {
                    std::cout << "need channel name." << std::endl;
                    break ;
                }
                tmp = user.getSpecificChannelJoined(msgs[1], *channelContainer);
                if (tmp == nullptr) {
                    std::cout << "ERROR" << std::endl;
                    break ;
                }
                sendMsg(user, command->topic(*tmp, msgs[1])); // 임시
                break;

            case MODE:
                //command->mode(content, content, content);
                /* test용 */
                //testCmd.push_back("test");
                //testCmd.push_back("+");
                command->mode(msgs, user, *channelContainer);
                break;

            default:
                std::cout << "[ COMMAND ERROR!!!!!!!!!!!!!! ]\n";
                break;
        }
    }
    catch(const std::exception& e) {
        std::cout << "-- [CATCH]: " << flag << " --\n";
        if (user.getUserIsPass() == 2 || user.getUserIsPass() == 3) { // 수정 요함
            std::cout << "disconnect User: " << flag << "\n";
            disconnectClient(&user);
        }
        std::cerr << "[Catch]: " <<  e.what() << '\n';
        sendMsg(user, e.what());
    }
    if ((flag == USER || flag == NICK) && userPass == 4 && user.getNickName() != "" && user.getUserName() != "") {
        user.setUserIsPass(5); // USER NICK 상관없이 register하는 예외처리
        sendMsg(user, ":irc.local 001 " + user.getNickName() + ":Welcome to the Localnet IRC Network " + user.getNickName() + "!root@127.0.0.1\n");
    }

	std::cout << "-----------------------------\n";
}



void Server::parser(std::string msg, User &user) {
	int			end;
    int         cr = 0;
    int         lf = 0;
    int         crlf = 0;
    int         lfcr = 0;
    int          i = 0;
    std::string temp;

    if (msg == "") {
        std::cout << "[parser] msg == \"\"\n";
        return ;
    }
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
        }
    }
    else if ((crlf = msg.find("\r\n", crlf)) != std::string::npos) {
        splitSpace(msg.substr(0, crlf), user);
        if ((crlf = msg.find("\r\n", crlf)) != std::string::npos) {
            parser(&msg[crlf + 2], user);
        }
    }
}

void    Server::splitSpace(std::string msg, User &user) {
    int i = 0;
    std::stringstream iss(msg);
    std::string str;
    std::vector <std::string> msgs;

    while (iss >> str) {
        msgs.push_back(str);
    }
    for (std::vector<std::string>::iterator iter = msgs.begin(); iter != msgs.end(); iter++) { // tester
        std::cout << "msg:" << *iter << "\n";
    }
    executeCommand(msgs, user);
}


void	Server::sendMsg(User &user, std::string &msg) const {
    if (msg == "")
        return ;
	write(user.getUserSock(), msg.c_str(), msg.length() + 1);
}

void	Server::broadcastMsg(User &user, Channel *channel, std::string &msg) const {
	std::vector<User *> userList;

    if (channel == nullptr)
        return ; // err 던지기
	userList = channel->getUserList();
	for (std::vector<User*>::iterator iter = userList.begin(); iter != userList.end(); iter++) {
        if ((**iter).getUserSock() != user.getUserSock())
            sendMsg(**iter, msg);
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