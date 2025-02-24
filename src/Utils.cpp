/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 12:36:38 by alli              #+#    #+#             */
/*   Updated: 2025/02/21 08:51:15 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"

int Serv::findClient(int client_fd)
{
	for (unsigned long i = 0; i < clients.size(); i++)
	{
		if (client_fd == clients[i].getFd())
			return (clients[i].getFd());
		else
			return -1;
	}
	return -1;
}

std::vector<std::string> Serv::splitStr(const std::string& str, std::string delim)
{
	std::vector<std::string> newList;
	// std::string item;
	int length = str.length();
	size_t start = 0;
	size_t end = 0;
	
	if (length == 0)
		return newList;
	while ((end = str.find(delim, start)) != std::string::npos )
	{
			newList.push_back(str.substr(start, end - start));
			start = end + delim.length();
	}
	newList.push_back(str.substr(start)); // adds the last element from the string
	return newList;
}

Client* Serv::getClientByFd(int client_fd) {
	for(const auto& [fd, client] : clients)
	{        
		if (client.getFd() == client_fd) {
            return const_cast<Client*>(&client);
        }
    }
    return nullptr;
}

Client* Serv::getClientByNickname(const std::string& nickname) {
    
	for(const auto& [fd, client] : clients)
	{
		if (client.getNickname() == nickname)
		{
		
			return const_cast<Client*>(&client);
		}
	}
    return nullptr;
}

std::shared_ptr<Channel> Serv::createChannel(const std::string& name)
{
	if (_channels.find(name) != _channels.end()) {
        std::cout << "Channel already exists.\n";
        return _channels[name]; 
	}
    std::shared_ptr<Channel> newChannel = std::make_shared< Channel>(name);
    _channels[name] = newChannel;
    std::cout << "Channel " << name << " created.\n";
    return newChannel;
}

// void Serv::sendError(int fd, const std::string& message, int errorCode) {
// 	 Client* client = getClientByFd(fd);
//     if (!client) {
//         std::cerr << "Client not found for fd: " << fd << std::endl;
//         return;
//     }

//     std::string errorResponse = ":" + client->getServerName() + " " + std::to_string(errorCode) + " " + client->getNickname() + " " + message + "\r\n";
    
//     // Debug output
//     std::cout << "Sending error to client " << fd << ": " << errorResponse;

//     ssize_t bytesSent = send(fd, errorResponse.c_str(), errorResponse.size(), 0);
//     if (bytesSent == -1) {
//         std::cerr << "Error sending error response to client " << fd << std::endl;
//     }
// 	else {
//         std::cout << "Successfully sent " << bytesSent << " bytes to client " << fd << std::endl;
//     }
// }

void Serv::sendError(int fd, const std::string& message, int errorCode) {
    std::string errorResponse = ":" + clients[fd].getServerName() + " " 
				+ std::to_string(errorCode) + " " + clients[fd].getNickname() 
				+ " " + message + "\r\n";
	std::cout<< "Error Response: "<< errorResponse<< std::endl;
    ssize_t bytesSent = send(fd, errorResponse.c_str(), errorResponse.size(), 0);
    if (bytesSent == -1) {
        std::cerr << "Error sending error response to client " << fd << std::endl;
    }
}

int Serv::checkChanName(std::string name)
{
	std::vector<char> chars = {'!', '@', '$', '%', ' '};
	for (char c : name)
	{
		if (std::find(chars.begin(), chars.end(), c) != chars.end() || \
				std::iscntrl(static_cast<unsigned char>(c))) {
            return 1;  // Invalid: contains forbidden character or control character
        }
	}
	return 0;
}