#include "Serv.hpp"
#include <csignal>

static Serv* server = NULL;
int check_port(int port)
{
    if(port < 1024 || port > 49151 )
        return(1);
    return(0);
}

static void cleanEverything()
{
    if (server) {
        delete server;
        server = NULL;
    }
}

void signalHandler(int signal) {
    std::cerr << "signal: " << signal << std::endl;
    cleanEverything();
    exit(signal);
}


int main(int argc, char **argv)
{
    std::signal(SIGINT, signalHandler);// Ctrl+C == 2
    std::signal(SIGQUIT, signalHandler); // quit
    std::signal(SIGSEGV, signalHandler); // Seg  == 11
    std::signal(SIGTERM, signalHandler); // Termin ==15
    try
    {
        if(argc !=3)
        {
            std::cout<< "Error! Usage ./ircserv PORT PASSWORD" <<std::endl;
            return(1);
        }
        int port = std::atoi(argv[1]);
        if(check_port(port) == 1)
        {
            std::cout << "Error! Port must be from 1024 to 49151"<<std::endl;
            return(1);
        }
          server = new Serv(port, argv[2]);
        server->launch();
        cleanEverything();

    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception caught: " << e.what() << '\n';
    }
    return 0;
}
