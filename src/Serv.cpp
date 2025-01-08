#include "Serv.hpp"

// Constructor
Serv::Serv(int port, std::string pass) : port(port), pass(pass), sock(nullptr)
{
    creating_socket();
}

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

void Serv::accepter() {
  //  std::cout << "Accepter" << std::endl;

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
    set_non_blocking(_new_socket); // new sockecke to non blocking

    ///////SASHA's NEW
    // Create a new Client object and add it to the clients list
    clients.push_back(Client(_new_socket));

    // Add the new socket to the poll list
    pollfd client_poll;
    client_poll.fd = _new_socket;
    client_poll.events = POLLIN;
    fds.push_back(client_poll);

}

void Serv::launch()
{
    // main server socket ... added to the iist of monitored fd
    pollfd server_poll;
    server_poll.fd = sock->get_sock(); 
    server_poll.events = POLLIN;/// monitore for incoom data
    fds.push_back(server_poll);

     while (true)
    {
        // wait fr vents on the monitored sockets
        int poll_res = poll(fds.data(), fds.size(), -1); // Wait indefinitely for events

        if (poll_res < 0)
        {
            perror("Poll failed");
            break;///if error exit the loop
        }
        for (size_t i = 0; i < fds.size(); ++i)
        {
            if (fds[i].revents & POLLIN) // If there is data to read in curr fd
            {
                //handle the server socket(new conn request)
                if (fds[i].fd == sock->get_sock())
                {
                    accepter();// accept new client conn
                    // if all good
                    if (_new_socket >= 0)
                    {
                        //add new client socket to the poll list
                        pollfd client_poll;
                        client_poll.fd = _new_socket;
                        client_poll.events = POLLIN;
                        fds.push_back(client_poll);
                    }
                }
                else
                {
                    //handle data for exist client
                    char buffer[1024];
                    int bytes_read = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                    if (bytes_read <= 0)
                    {
                        if (bytes_read == 0)//  nothing to read
                        {
                            // disconneted
                            std::cout << "\033[33mClient disconnected: FD " << fds[i].fd << "\033[0m" << std::endl;
                        }
                        else
                        {
                            perror("Recv failed");// connot be -
                        }
                        // close CLient socket and remove i form the poll list
                        close(fds[i].fd);
                        fds.erase(fds.begin() + i);
                        // Find and remove client from the `clients` vector
                        for (size_t j = 0; j < clients.size(); ++j)
                        {
                            if (clients[j].get_fd() == fds[i].fd)
                            {
                                clients.erase(clients.begin() + j);
                                break;
                            }
                        }
                        --i;// adjust index to account
                    }
                    else
                    {
                        // process recived data
                        buffer[bytes_read] = '\0';
                        std::cout << "\033[36mReceived from FD " << fds[i].fd << ": " << buffer << "\033[0m" << std::endl;

                        // Echo the data back to the client
                        send(fds[i].fd, buffer, bytes_read, 0);
						
						std::string client_input(buffer);
						std::stringstream ss(client_input);
						std::string line;
						
						while (getline(ss, line))
						{
							if (line.empty())
								continue;
							else
								parse_command(fds[i].fd, line);
						}

                    }
                }
            }
        }
    }
}