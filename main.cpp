#include "./source/Server.hpp"

int main(int argc, char **argv)
{
	// if (argc != 3) {
	// 	std::cout << "argv error\n";
	// }
	if (argc < 2) {
		std::cout << "plz password\n";
		return 1;
	}
	std::cout << "server pass :" << argv[1] << "\n";
	Server server(argv[1]);
	server.serverStart();
}