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
    sock = new Socket(AF_INET, SOCK_STREAM, 0,port);

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
    int non_block = fcntl(sock_fd, F_GETFL, 0);// checking the access to socket, 
    // if it is < 0 then fcnl failed
    if (non_block < 0)
    {
        perror("fcntl get failed");
        exit(EXIT_FAILURE);
    }
    if (fcntl(sock_fd, F_SETFL, non_block | O_NONBLOCK) < 0)// makes socket non blocking by
    // O_NONBLOCK flag
    // F_SETFL 
    {
        perror("fcntl set non-blocking failed");
        exit(EXIT_FAILURE);
    }
}

void Serv::send_message(int client_fd, const std::string& message)
{
   std::string messages = message + "\r\n";
    send(client_fd, messages.c_str(), messages.length(), 0);
}


void Serv::accepter() {

    Socket* socket = sock;
    if (socket == nullptr)
        return;
    int sock_fd = socket->get_sock();
    if (sock_fd < 0)
        return;
        //prepare to accept new connection
    struct sockaddr_in address = socket->get_address();
    socklen_t adrlen = sizeof(address);

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
        return;
    }
    if(_new_socket >=0)
    {
        set_non_blocking(_new_socket); // new sockecke to non blocking

        Client new_cl(_new_socket);
        clients[_new_socket] = new_cl;

        // Add the new socket to the poll list
        pollfd client_poll;
        client_poll.fd = _new_socket;
        client_poll.events = POLLIN;
        fds.push_back(client_poll);

        std::string server_name = "ircserv";
       // Retrieve the nickname
        std::string nick = "Guest";  // Default fallback nickname
		if (clients.find(_new_socket) != clients.end()) {
			Client& client = clients[_new_socket];
            nick = client.getNickname(); 
			} // Retrieve client nickname
    }
}