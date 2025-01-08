/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 13:17:16 by alli              #+#    #+#             */
/*   Updated: 2025/01/07 14:19:41 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Serv.hpp"

std::string Client::getUsername() {
	return (Client::_username);
}

std::string Client::getNickname() {
	return (Client::_nickname);
}
std::string Client::getPassword() {
	return (Client::_password);
}
//Sasha added can be changed/deleted 
Client::~Client() {
    // Optional: Clean up resources if needed
}
// std::vector<std::string> Client::getServerInfo() {
	
// }