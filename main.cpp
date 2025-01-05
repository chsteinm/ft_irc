#include "Server.hpp"

void	sigHandler(int sig) {
	if (sig == SIGINT)
		throw std::runtime_error("Program killed");
}	

int	main(int argc, char **argv) {
	if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return EXIT_FAILURE;
    }
	std::istringstream iss(argv[1]);
    int port;
    if (!(iss >> port)) {
        std::cerr << "Error: Invalid port number." << std::endl;
        return 1;
    }
	try {
		Server server(port, argv[2]);
		signal(SIGINT, &sigHandler);
	}
	catch(std::exception &e)
    {
        std::cout << RED << "/!\\ "<< e.what() << END<<std::endl;
    }
	return 0;
}