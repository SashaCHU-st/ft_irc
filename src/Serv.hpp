#pragma once
#include "Socket.hpp"
#include "Signal.hpp"
#include <vector>
#include <poll.h>
#include <iostream>
#include <exception>
#include <fcntl.h>
#include <cstdlib>
#include <sstream>

class Serv
{
    private:
        int port;
        int _new_socket;
        std::string pass;
        Socket* sock;
         std::vector<pollfd> fds;
    public:
        //construc
        Serv(int port, std::string pass);
        ~Serv();

        ///functions
        void creating_socket();
        int get_port() const;
        void set_non_blocking(int sock_fd);
        void accepter();
        void launch();
		void parse_command(int client_fd, std::string line);
};