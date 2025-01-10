/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 13:17:16 by alli              #+#    #+#             */
/*   Updated: 2025/01/09 13:37:01 by alli             ###   ########.fr       */
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
int	Client::getFd() const {
	return this->_fd;
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