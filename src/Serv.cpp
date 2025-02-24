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
    if(_new_socket >=0)
    {
        set_non_blocking(_new_socket); // new sockecke to non blocking

        Client new_cl(_new_socket);
// std::cout << "Sending welcome message to: " << nick << std::endl;

     //   new_cl.setNickname("wwww");

        ///////SASHA's NEW
        // Create a new Client object and add it to the clients list
        // clients.push_back(Client(_new_socket));
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
        // for (const Client& client : clients) {
        //     if (client.getFd() == _new_socket) {
                nick = client.getNickname(); 
				} // Retrieve client nickname
            // std::cout << "Sending welcome message to: " << nick << std::endl;

    //             break;
			// std::string message = " :ircserver 001 " + clients[sock_fd].getNickname() + " :Welcome to the IRC Network, " + nick + "!";
        	// send_message(_new_socket, message);  // Send the message
            }

        }

        // welcome message

    // }



void Serv::launch()
{
    //     // main server socket ... added to the iist of monitored fd
    pollfd server_poll;
    server_poll.fd = sock->get_sock(); 
    server_poll.events = POLLIN;  /// monitore for incoom data
    fds.push_back(server_poll);

    while (true)
    {
        // wait fr vents on the monitored sockets
        int poll_res = poll(fds.data(), fds.size(), -1);// Wait indefinitely for events
        if (poll_res < 0)
        {
            perror("Poll failed");
            break;///if error exit the loop
        }

        for (size_t i = 0; i < fds.size(); ++i)
        {
            if (fds[i].revents & POLLIN)  // If there is data to read in curr fd
            {
                //handle the server socket(new conn request)
                if (fds[i].fd == sock->get_sock())  
                {
                    accepter();
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
                else  //handle data for exist client
                {
                    char buffer[1024] = {0};
                    int bytes_read = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);

                    if (bytes_read < 0)
                    {
                        // Check for EAGAIN or EWOULDBLOCK
						std::cout << "bytes < 0" << std::endl;
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            continue;  // No data yet, try again later
                        } else {
                            // perror("Recv failed");
                              close(fds[i].fd);
                            fds.erase(fds.begin() + i);
                            --i; // Adjust index
                            continue;
                        }
                    }
                    else if (bytes_read == 0)  
                    {
                        // Connection closed by the client
                        std::cout << "Client disconnected: FD " << fds[i].fd << "\n";
                        close(fds[i].fd);
                        fds.erase(fds.begin() + i);
                        for (size_t j = 0; j < clients.size(); ++j)
                        {
                            if (clients[j].getFd() == fds[i].fd)
                            {
                                clients.erase(fds[i].fd);
                                break;
                        }
                    }
                        --i;
                        continue;
                    }
                    else
                    {
						std::cout << "before receiving data" << std::endl;
                        buffer[bytes_read] = '\0';
                        std::cout << "Received data from FD " << fds[i].fd << ": " << buffer << std::endl;
                        _clientBuffers[fds[i].fd] += buffer;

                        size_t pos;
                        while ((pos = _clientBuffers[fds[i].fd].find("\n")) != std::string::npos) 
                        {
							std::cout << "outer message" << std::endl;
                            std::string command = _clientBuffers[fds[i].fd].substr(0, pos);
                            _clientBuffers[fds[i].fd].erase(0, pos + 1);

                            if (!command.empty() && command.back() == '\r') {
                                command.pop_back();
                            }

                            std::stringstream ss(command);
                            std::string line;
                            while (getline(ss, line)) 
                            {
								std::cout << "start loop" << std::endl;
                                if (line.empty())
                                    continue;
                                if (parse_command(fds[i].fd, line) == 1)
									break;
								std::cout << "finished loop" << std::endl;
                            }
                        }
                        if (sendWelcomeMsg(fds[i].fd) == 1)
							break;
                    }
					std::cout << "waiting for next command" << std::endl;
                }
            }
        }
    }
}
