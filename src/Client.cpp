/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 13:17:16 by alli              #+#    #+#             */
/*   Updated: 2025/01/08 11:47:05 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Serv.hpp"

Client::Client() {}
Client::~Client() {}

std::string Client::getUsername() {
	return (Client::_username);
}

std::string Client::getNickname() {
	return (Client::_nickname);
}
std::string Client::getPassword() {
	return (Client::_password);
}

// std::vector<std::string> Client::getServerInfo() {
	
// }