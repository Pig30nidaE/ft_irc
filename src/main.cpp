#include "../include/Server.hpp"


int main(int argc, char **argv)
{
	if (argc != 3) {
		std::cout << "ERROR: NEEDMOREPARMS\n";
		std::cout << "plz ./ircserver [port] [pass]\n";
		return 1;
	}
	std::cout << "server port :" << argv[1] << "\n";
	std::cout << "server pass :" << argv[2] << "\n";
	Server server(argv[1], argv[2]);
	server.serverStart();
}