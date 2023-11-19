#include "./server.hpp"


Server::Server(void) {
    struct sockaddr_in server_addr;

    if ((server_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        exitWithPerror("socket() error\n" + std::string(strerror(errno)));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(8080);

    // 서버 소켓 바인드
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        exitWithPerror("bind() error\n" + std::string(strerror(errno)));

    // 서버 소켓 listen 설정
    if (listen(server_socket, MAXVISITE) == -1)
        exitWithPerror("listen() error\n" + std::string(strerror(errno)));
    // 서버 소켓 NonBlocking 설정
    fcntl(server_socket, F_SETFL, O_NONBLOCK);
    
    /* init kqueue */
    // kqueue 생성
    if ((kq = kqueue()) == -1)
        exitWithPerror("kqueue() error\n" + std::string(strerror(errno)));


	std::cout << "Server constructor!!\n";
}

void Server::serverStart(void) {

    int new_events;
	std::vector<struct kevent> change_list;
    struct kevent* curr_event;
    
    changeEvents(change_list, server_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    while (1)
    {
        sleep(1);
        /*  apply changes and return new events(pending events) */
        new_events = kevent(kq, &change_list[0], change_list.size(), event_list, 8, NULL);
        if (new_events == -1)
            exitWithPerror("kevent() error\n" + std::string(strerror(errno)));

        // kqueue에 등록은 한 번만하면 되고 fd를 close()하면 참조된 kevent들이 사라진다. 
        change_list.clear(); // clear change_list for new changes
        
        for (int i = 0; i < new_events; ++i)
        {
            curr_event = &event_list[i];

            /* check error event return */
            if (curr_event->flags & EV_ERROR)
            {
                if (curr_event->ident == server_socket)
                    exitWithPerror("server socket error");
                else
                {
                    std::cerr << "client socket error" << std::endl;
                    disconnectClient(curr_event->ident, clients);
                }
            }
            else if (curr_event->filter == EVFILT_READ)
            {
                if (curr_event->ident ==server_socket) // test
                    std::cout << "server_socket!!";
                std::cout << "[" << i << "]" << ": READ MODE!!\n";  // test
                if (curr_event->ident ==server_socket)
                {
                    /* accept new client */
                    int client_socket;
                    if ((client_socket = accept(server_socket, NULL, NULL)) == -1)
                        exitWithPerror("accept() error\n" + std::string(strerror(errno)));
                    std::cout << "accept new client: " << client_socket << std::endl;
                    fcntl(client_socket, F_SETFL, O_NONBLOCK);

                    /* add event for client socket - add read && write event */
                    changeEvents(change_list, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    changeEvents(change_list, client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    clients[client_socket] = "";
                }
                else if (clients.find(curr_event->ident)!= clients.end())
                {
                    /* read data from client */
                    char buf[1024];
                    int n = read(curr_event->ident, buf, sizeof(buf));

                    if (n <= 0)
                    {
                        if (n < 0)
                            std::cerr << "client read error!" << std::endl;
                        disconnectClient(curr_event->ident, clients);
                    }
                    else
                    {
                        buf[n] = '\0';
                        clients[curr_event->ident] += buf;
                        std::cout << "received data from " << curr_event->ident << ": " << clients[curr_event->ident] << std::endl;
                    }
                }
            }
            else if (curr_event->filter == EVFILT_WRITE)
            {
                std::cout << "[" << i << "]" << ": WRITE MODE!!\n"; 
                /* send data to client */
                std::map<int, std::string>::iterator it = clients.find(curr_event->ident);
                if (it != clients.end())
                {
                    if (clients[curr_event->ident] != "") // 모든 client_socket은 여기서 걸리짐 조건문 바꾸면 채널별로 넣을 수 있을듯
                    {
                        int n;
                        if ((n = write(curr_event->ident, clients[curr_event->ident].c_str(),
                                        clients[curr_event->ident].size()) == -1))
                        {
                            std::cerr << "client write error!" << std::endl;
                            disconnectClient(curr_event->ident, clients);  
                        }
                        else
                            clients[curr_event->ident].clear();
                    }
                }
            }
        }

    }
    return ;
}


Server::~Server() {
    for (std::map<int, std::string>::iterator iter = clients.begin(); iter != clients.end(); iter++) {
        if (close(iter->first) == -1)
            std::cout << "client socket close ERROR\n";
           
    }
    if (close(kq) == -1)
        std::cout << "Kqueue close ERROR\n";
    if (close(server_socket) == -1)
        std::cout << "Kqueue close ERROR\n";
	std::cout << "server destroyed!!\n";
}

void Server::disconnectClient(int client_fd, std::map<int, std::string>& clients)
{
    std::cout << "client disconnected: " << client_fd << std::endl;
    close(client_fd);
    clients.erase(client_fd);
}


void Server::changeEvents(std::vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
    struct kevent temp_event;

    EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    change_list.push_back(temp_event);
}


void Server::exitWithPerror(const std::string& msg)
{
    std::cerr << msg << std::endl;
    exit(EXIT_FAILURE);
}


