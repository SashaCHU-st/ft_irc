#include "Serv.hpp"
#include "Channel.hpp" 

// Constructor
Serv::Serv(int port, std::string pass) : port(port), pass(pass), sock(nullptr)
{
    creating_socket();
}

std::map<std::string, std::shared_ptr<Channel>> Serv::_channels;

// Destructor
Serv::~Serv()
{
    delete sock;
}

int Serv::get_port() const
{
    return port;
}
// Creating a socket
void Serv::creating_socket()
{
    sock = new Socket(AF_INET, SOCK_STREAM, 0,port);///0 protocol so =>SOCK_STREAM

    if (!sock || sock->get_sock() < 0)
    {
        std::cerr << "Failed to create socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "\033[32m" << "ALL GOOD we can continue with launching" << "\033[0m" << std::endl; 
}
std::string Serv::get_pass() {
	return pass;
}

///NON BLOCKING-
void Serv::set_non_blocking(int sock_fd)
{
                                                        // fcntl(int fd, int cmd, and othe diff arguments);
    int non_block = fcntl(sock_fd, F_SETFL, O_NONBLOCK);// checking the access to socket, 
                                                        // makes socket non blocking by
                                                        // O_NONBLOCK flag                                               
                                                        // F_SETFL 
    if (non_block < 0)
    {
        perror("fcntl get failed");
        exit(EXIT_FAILURE);
    }
}

void Serv::send_message(int client_fd, const std::string& message)
{
    std::string messages = message + "\r\n";
    send(client_fd, messages.c_str(), messages.length(), 0);
}
void Serv::accepter()// accepting new client connection
{
    if (!sock)  
        return;

    int sock_fd = sock->get_sock();
    if (sock_fd < 0)
        return;
    //prepare to accept new connection
    // struct sockaddr_in address = socket->get_address();
    // socklen_t adrlen = sizeof(address);
    struct sockaddr_in address;
    socklen_t adrlen = sizeof(address);

    memset(&address, 0, sizeof(address));// initializes the structure to zero to avoid garbage values
    // accept new CLIENT connect
    _new_socket = accept(sock_fd, (struct sockaddr*)&address, &adrlen);
    // listen socket to accept incom conn req from CLIENT
    // creates newsocket and return a fd for new socket
    // the original "big socket" will remain open and contuue listen for new incomes
    if (_new_socket < 0) 
    {
        /// No pending connections (expected in non-blocking mode)
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            std::cout << "No pending connections. Non-blocking accept returned." << std::endl;
        else
            perror("Failed to accept connection");

        _new_socket = -1;
        return;
    }

    set_non_blocking(_new_socket); // new socket to non blocking
    clients[_new_socket] = Client(_new_socket);
    _clientBuffers[_new_socket] = "";

    // Add the new socket to the poll list
    pollfd client_poll = {}; 
    client_poll.fd = _new_socket;
    client_poll.events = POLLIN;
    fds.push_back(client_poll);

    // Retrieve the nickname
    std::string nick = "Guest";  // Default fallback nickname
    if (clients.find(_new_socket) != clients.end()) {
        Client& client = clients[_new_socket];
        nick = client.getNickname();  
    }// Retrieve client nickname
}
