#pragma once

#include <sys/wait.h>
#include <netinet/in.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <poll.h>
#include <sys/socket.h>

class Socket
{
private:
    int sock;              
    int conn;
    struct sockaddr_in address;

    int domain;
    int type;
    int protocol;
    int port;
    // std::vector<struct pollfd> *fds;/// for dinamcalyy fds

public:
    // consctructors
    Socket(int domain, int type, int protocol, int port);

    //desctruc
    //~Socket();
    //getters
    struct sockaddr_in get_address();
    int get_sock();
    int get_conn();
    std::vector<pollfd> *get_fds();

    // Setters
    void set_conn(int conn);

//functuons
    int socketing(int port);
    int binding();
    int listening();
    int run(int port);
};