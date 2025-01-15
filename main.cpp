#include "Server.hpp"

int	main(int argc, char **argv) {
	if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return EXIT_FAILURE;
    }
	std::istringstream iss(argv[1]);
    int port;
    if (!(iss >> port) || port < 0) {
        std::cerr << "Error: Invalid port number." << std::endl;
        return 1;
    }
	try {
		Server server(port, argv[2]);
	}
	catch(std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
	return 0;
}