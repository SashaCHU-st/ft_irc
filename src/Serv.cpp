#include "Serv.hpp"

// Constructor
Serv::Serv(int port, std::string pass) : port(port), pass(pass), sock(nullptr)
{
    creating_socket();
}

// Destructor
Serv::~Serv()
{
    // Clean up the allocated socket
    delete sock;
}


int Serv::get_port() const
{
    return port;
}
// Creating a socket
void Serv::creating_socket()
{
    sock = new Socket(AF_INET, SOCK_STREAM, 0,port);

    if (!sock || sock->get_sock() < 0)
    {
        std::cerr << "Failed to create socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "\033[32m" << "ALL GOOD we can continue with launching" << "\033[0m" << std::endl; 
}

void Serv::launch()
{

}