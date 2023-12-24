#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define PORT 8080

int main(int argc, char const *argv[]) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    const char* hello = "Hello from client";
    char buffer[1024] = {0};

    // Create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }
    std::string say = "PASS 123\r\nNICK donghak\r\nUSER guest tolmoon tolsun :Ronnie Reagan\r\n";
	// std::cin >> say;
	// while (say != "end")
	// {
    std::cout << "say: " << say << "\n";
        // send(sock, say, strlen(say) + 1, 0);
        // send(sock, &say, say.length() + 1, 0);
        send(sock, "PASS 123\r\nNICK donghak\r\nUSER guest tolmoon tolsun :Ronnie Reagan\r\n", say.length() + 1, 0);
        std::cout << "to server: " << say << std::endl;
        // say = "";
        valread = read(sock, buffer, 1024);
        std::cout << "read msg: " << buffer << std::endl;
		// std::cin >> say;
	// }


    // Receive message from the server

    close(sock);
    return 0;
}
