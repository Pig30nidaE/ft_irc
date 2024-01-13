#include "../include/Server.hpp"


int main(int argc, char **argv)
{
	if (argc != 3) {
		std::cerr << "ERROR: NEEDMOREPARMS\n";
		std::cerr << "plz ./ircserver [port] [pass]\n";
		return 1;
	}
	if (!(6660 <= std::atoi(argv[1]) && std::atoi(argv[1]) <= 6669)) {
		std::cerr << "ERROR: port range 6660 ~ 6669" << std::endl;
		return 1;
	}
	Server server(argv[1], argv[2]);
	server.serverStart();
}