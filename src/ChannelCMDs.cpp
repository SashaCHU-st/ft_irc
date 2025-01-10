/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelCMDs.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: epolkhov <epolkhov@student.42.fr>          #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-01-10 12:59:50 by epolkhov          #+#    #+#             */
/*   Updated: 2025-01-10 12:59:50 by epolkhov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Serv.hpp"


int Serv::cmdJOIN(int fd, std::string name)
{
	if (name.empty())
	{
		std::cout<<"No parameter after JOIN"<< std::endl;
		return 1;
	}
	if (name[0] != '#')
	{
		std::cout<< "Channel name must start with #."<< std::endl;
		return 1;
	}
	if (_channels.find(name) != _channels.end())
	{
		std::cout<< "Channel already exists." << std::endl;
		return 1;
	}
	
	Channel newChannel(name);
	_channels[name] = newChannel;
	std::cout<< "New channel: "<< newChannel.getName() << " created."<<std::endl;
	
	std::string defaultTopic = "No topic set";
	newChannel.setTopic(defaultTopic, nullptr);

	Client* client = getClientByFd(fd);
	if (!client) {
        std::cout << "Client not found for fd: " << fd << std::endl;
        return 1;
    }
	if (newChannel.isUserInChannel(client)) {
        std::cout << "User is already in the channel." << std::endl;
        return 1;
    }
	newChannel.addUser(client);

	return  0;
}