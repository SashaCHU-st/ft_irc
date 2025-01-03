#include "Serv.hpp"

// Constructor
Serv::Serv(int port, std::string pass) : port(port), pass(pass), sock(nullptr)
{
    creating_socket();
}

// Creating a socket
void Serv::creating_socket()
{
    int domain = AF_INET;
    int type = SOCK_STREAM;
    int protocol = 0;
    u_long interface = INADDR_ANY;
    int backlog = 10;

    // Allocate memory for the Socket object
    sock = new Socket(domain, type, protocol, interface, port, backlog);

    if (!sock || sock->get_sock() < 0)
    {
        std::cerr << "Failed to create socket" << std::endl;
        exit(EXIT_FAILURE);
    }
}

// Destructor
Serv::~Serv()
{
    // Clean up the allocated socket
    delete sock;
}
