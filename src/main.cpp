#include "Serv.hpp"
#include <csignal>

static Serv* kuku = NULL;
int check_port(int port)
{
    if(port < 1024 || port > 9999)///???
        return(1);
    return(0);
}

static void cleanEverything()
{
    if (kuku) {
        delete kuku;
        kuku = NULL;
    }
}

void signalHandler(int signal) {
    std::cerr << "signal: " << signal << std::endl;
    cleanEverything();
    exit(signal);
}

void signals()
{
    signal(SIGSEGV, signalHandler); // Seg  == 11
    signal(SIGINT, signalHandler); // Ctrl+C == 2
    signal(SIGTERM, signalHandler); // Termin ==15
} 

int main(int argc, char **argv)
{
    std::signal(SIGINT, signalHandler);
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
            std::cout << "Error! Port must be from !!!!!! to !!!!!!"<<std::endl;
            return(1);
        }
          kuku = new Serv(port, argv[2]);
        kuku->launch();
        cleanEverything();

    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception caught: " << e.what() << '\n';
    }
    return 0;
}
