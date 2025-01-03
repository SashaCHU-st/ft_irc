#pragma once
#include "Socket.hpp"
#include "Signal.hpp"
#include <iostream>
#include <exception>
#include <cstdlib>

class Serv
{
    private:
        int port;
        std::string pass;
        Socket* sock; 
    public:
        //construc
        Serv(int port, std::string pass);
        ~Serv();

        ///functions
        void creating_socket();
};