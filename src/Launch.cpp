#include "Serv.hpp"

void Serv::launch()
{
    // main server socket ... 
    //init pollfd for Server socket
    pollfd server_poll;//creating pollfd stcructure for monitoring the server socket
    server_poll.fd = sock->get_sock(); //gets the server's "main socket fd"
    server_poll.events = POLLIN;  /// monitore for income data (new connectiobs)
                                    //flag that tell poll to minitor the socket for incoming connect
    fds.push_back(server_poll);//added to the iist of monitored fd

    while (true)
    {
        // wait for events on the monitored sockets
        int poll_res = poll(fds.data(), fds.size(), -1);// Wait indefinitely for events
                                                        //pointing for pollfds array
        if (poll_res < 0)
        {
            perror("Poll failed");
            break;
        }
        for (size_t i = 0; i < fds.size(); ++i)/// go throuh all fds
        {
            
            if (fds[i].revents & POLLIN)  // If there is data to read in curr fd
            {
                //handle the server socket(new conn request)
                if (fds[i].fd == sock->get_sock())  
                {
                    _new_socket = -1;
                    accepter();
                    if (_new_socket >= 0)// if new client has connected
                    {
                         //add new client socket to the poll list
                        pollfd client_poll = {};
                        client_poll.fd = _new_socket;
                        client_poll.events = POLLIN;
                        fds.push_back(client_poll);
                    }
                }
                else  //handle data for exist client
                {
                    char buffer[1024] = {0};
                    int bytes_read = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);//reads data from the client
                    if (bytes_read < 0)
                    {
                        // Check for EAGAIN or EWOULDBLOCK
                        //EAGAIN or EWOULDBLOCKThe socket is non-blocking, and there’s no data available.
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            continue;  // No data yet, try again later
                        } else {
                            close(fds[i].fd);
                            fds.erase(fds.begin() + i);
                            --i; 
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
                        buffer[bytes_read] = '\0';
                        std::cout << "Received data from FD " << fds[i].fd << ": " << buffer << std::endl;
                        _clientBuffers[fds[i].fd] += buffer;

                        size_t pos;
                        while ((pos = _clientBuffers[fds[i].fd].find("\n")) != std::string::npos) 
                        {
                            std::string command = _clientBuffers[fds[i].fd].substr(0, pos);
                            _clientBuffers[fds[i].fd].erase(0, pos + 1);

                            if (!command.empty() && command.back() == '\r') {
                                command.pop_back();
                            }

                            std::stringstream ss(command);
                            std::string line;
                            while (getline(ss, line)) 
                            {
                                if (line.empty())
                                    continue;
                                if (parse_command(fds[i].fd, line) == 1)
									break;
                            }
                        }
                        if (sendWelcomeMsg(fds[i].fd) == 1)
							break;
                    }
                }
            }
        }
    }
}

