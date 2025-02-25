#include "Socket.hpp"

Socket::Socket(int domain, int type, int protocol, int port)
    : domain(domain), type(type), protocol(protocol), port(port)
{
    if (run(port) != 0)
    {
        std::cerr << "Failed to initialize socket on port " << port << std::endl;
        exit(EXIT_FAILURE);
    }
}

Socket::~Socket()
{

};

int Socket::get_conn()
{ 
    return conn;
}


int Socket::get_sock() const
{
    return sock;
}


struct sockaddr_in Socket::get_address()
{
    return address;
}

void Socket::set_conn(int conn)
{
    this->conn = conn;
}

int Socket::socketing(int port)
{
    sock = socket(domain, type, protocol); // AF_INET, SOCK_STREAM, 0 ( 0 default based on type if SOCK_STREAM => TCP)
                                         //SOK_DGRAM =>UDP
    if (sock < 0)
    {
        perror("Failed");
        return(1);
    }
    int opt =1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("Set socket options failed");
        close(sock);
        return 1;
    }
    address.sin_family = domain;// AF_INET
    address.sin_port = htons(port);// Port 4051
    address.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY (0.0.0.0)
                                            // option that do not need to wait when again run server
    return(0);
}
int Socket::binding()
{
    int binding_from_server = bind(get_sock(), (struct sockaddr *)&address, sizeof(address));
    if(binding_from_server)
    {
        perror("failed");
        return(1);
    }
    set_conn(binding_from_server);
    if(get_conn() < 0)
    {
        perror("failed");
        return(1);
    }
    return(0);
}
int Socket::listening()
{
    int listening_status = listen(sock, 10); // backlog is the max number of pending connections
                                            //listening incoming con  from client
    if (listening_status < 0)
    {
        perror("failed");
        return(1);
    }
    return(0);
}

int Socket::run(int port)
{
    if(socketing(port) !=0 )
        return(1);
    if(binding()!= 0)
        return(1);
    if(listening() != 0)
        return(1);
   std::cout << "\033[32m" << "ALL GOOD sockets" << "\033[0m" << std::endl; 
    return(0);
}



