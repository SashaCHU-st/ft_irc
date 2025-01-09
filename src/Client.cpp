/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 13:17:16 by alli              #+#    #+#             */
/*   Updated: 2025/01/09 10:35:20 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Serv.hpp"

Client::Client() {}
Client::~Client() {}

std::string Client::getUsername() {
	return this->_username;
}

std::string Client::getNickname() {
	return this->_nickname;
}
std::string Client::getPassword() {
	return this->_password;
}
int	Client::getFd() {
	return this->_fd;
}

void Client::setNickname(std::string nickname) {
	// if (nickname.empty())
	// {
	// 	std::cerr << "Please input a nickname" << std::endl;
	// 	return;
	// }
	_nickname = nickname;
}
void Client::setUsername(std::string username) {
	if (username.empty())
	{
		std::cerr << "Please input a username" << std::endl;
		return;
	}
	_username = username;
}
void Client::setFd(int fd) {
	_fd = fd;
}

// std::vector<std::string> Client::getServerInfo() {
	
// }