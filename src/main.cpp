#include "Serv.hpp"

int check_port(int port)
{
    if(port < 1024 || port > 9999)///???
        return(1);
    return(0);
}

int main(int argc, char **argv)
{
    // (void)argc;
    Signal sig;
    try
    {
        if(argc !=3)
        {
            std::cout<<"Error! Usage ./ircserv PORT PASSWORD"<<std::endl;
            return(1);
        }
        int port = std::atoi(argv[1]);
        if(check_port(port) == 1)
        {
            std::cout<<"Error! Port must be from !!!!!! to !!!!!!"<<std::endl;
            return(1);
        }
        Serv serv(port, argv[2]);
        serv.launch();

    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception caught: " << e.what() << '\n';
    }
    return 0;
}
