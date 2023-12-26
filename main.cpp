#include "./source/Server.hpp"

int main(int argc, char **argv)
{
	if (argc != 3) {
		std::cout << "ERROR: NEEDMOREPARMS\n";
	}
	std::cout << "server pass :" << argv[2] << "\n";
	std::cout << "server port :" << argv[1] << "\n";
	Server server(argv[1], argv[2]);
	server.serverStart();
}