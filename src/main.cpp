#include "../include/server.hpp"

int main(int ac, char **av){

    if (ac != 3) {
        std::cerr << "Usage:\t./ircserv [port] [password]." << std::endl;
        return (EXIT_FAILURE);
    }

    std::string pwd(av[2]);
    if (!av[1][0]){
        std::cerr << "Empty port."<< std::endl;
        std::cerr << "Usage:\t./ircserv [port] [password]." << std::endl;
        return (EXIT_FAILURE);
    }
    std::stringstream ss(av[1]);
    int port;
    ss >> port;
    if (!ss.eof()) {
        std::cerr << "Usage:\t./ircserv [port] [password]." << std::endl;
        return (EXIT_FAILURE);
    }
    if (port < 1024) {
        std::cerr << "Port needs to be at least 1024." << std::endl;
        return (EXIT_FAILURE);
    }
    try {
        Server serv(port, pwd);
        serv.run();
    }
    catch (const std::exception &e){
        std::cerr << e.what() << std::endl;
        return (EXIT_FAILURE);
    }
    return 0;
}