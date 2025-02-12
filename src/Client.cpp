/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 13:17:16 by alli              #+#    #+#             */
/*   Updated: 2025/01/14 15:02:55 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Client.hpp"
#include "Serv.hpp"

Client::Client() {}
Client::Client(int fd) : _password(""), _fd(fd) {}
Client::~Client() {}

std::string Client::getUsername() {
	return this->_username;
}

std::string Client::getNickname() const {
	return this->_nickname;
}
std::string Client::getPassword() {
	return this->_password;
}
std::string Client::getHostName() {
	return this->_hostname;
}
int	Client::getFd()  const {
	return this->_fd;
}

std::string Client::getServerName()
{
	return _servername;
}

std::vector<std::shared_ptr<Channel>> Client::getChannels() {
    return _joinedChannels;
}

void Client::setHostName(std::string hostname){
	_hostname = hostname;
}
void Client::setNickname(std::string nickname) {
	_nickname = nickname;
}
void Client::setServername(std::string servername) {
	_servername = servername;
}
void Client::setUsername(std::string username) {
	// if (username.empty())
	// {
	// 	std::cerr << "Please input a username" << std::endl;
	// 	return;
	// }
	_username = username;
}
void Client::setRealname(std::string names)
{
	_realname.push_back(names);
}
void Client::setFd(int fd) {
	_fd = fd;
}
// int Client::get_fd() const 
// { 
// 	return _fd;
// }
// std::vector<std::string> Client::getServerInfo() {
	
// }


// Channel Part

void Client::joinChannel(std::shared_ptr<Channel> channel) {
    if (std::find(_joinedChannels.begin(), _joinedChannels.end(), channel) == _joinedChannels.end()) {
        _joinedChannels.push_back(channel);
        std::cout << _nickname<<" joined the channel:  " << channel->getName() << std::endl;
    } else {
        std::cout << "User is already in the channel: " << channel->getName() << std::endl;
    }
}

// void Client::leaveChannel(std::shared_ptr<Channel> channel) {
//     auto it = std::find(_joinedChannels.begin(), _joinedChannels.end(), channel);
//     if (it != _joinedChannels.end()) {
//         _joinedChannels.erase(it);
//         std::cout << "You leaved the channel: " << channel->getName() << std::endl;
//     } else {
//         std::cout << "Channel not found in client's joined channels.\n";
//     }
// }

void Client::leaveChannel(const std::string& channelName) {
    auto it = std::find_if(_joinedChannels.begin(), _joinedChannels.end(),
        [&channelName](const std::shared_ptr<Channel>& ch) {
            return ch->getName() == channelName;
        });

    if (it != _joinedChannels.end()) {
        std::cout << "You left the channel: " << (*it)->getName() << std::endl;
        _joinedChannels.erase(it);
    } else {
        std::cout << "Channel not found in client's joined channels.\n";
    }
}

const std::vector<std::shared_ptr<Channel>>& Client::getJoinedChannels() const {
    return _joinedChannels;
}

