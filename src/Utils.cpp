/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 12:36:38 by alli              #+#    #+#             */
/*   Updated: 2025/01/09 13:32:54 by alli             ###   ########.fr       */
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
std::vector<std::string> Serv::splitStr(const std::string& str, std::string delim) {
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
	// for (Client& client : clients) {
    //     if (client.getNickname() == nickname) {
    //         return &client; // Return a pointer to the matching client
    //     }
	}
    return nullptr; // Return nullptr if no client matches the nickname
}

std::shared_ptr<Channel> Serv::createChannel(const std::string& name)
{
	if (_channels.find(name) != _channels.end()) {
            std::cout << "Channel already exists.\n";
            return _channels[name]; // Return the existing channel
        }

        std::shared_ptr<Channel> newChannel = std::make_shared< Channel>(name);
        _channels[name] = newChannel;
        std::cout << "Channel " << name << " created.\n";
        return newChannel;
}