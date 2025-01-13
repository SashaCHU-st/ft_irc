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

#include "Serv.hpp"

int Serv::cmdJOIN(int fd, std::string name)
{
	if (name.empty())
	{
		std::cout<<"No parameter after JOIN"<< std::endl;// ERR_NEEDMOREPARAMS (461)
		return 1;
	}
	if (name[0] != '#')
	{
		std::cout<< "Channel name must start with #."<< std::endl;// ERR_NOSUCHCHANNEL(403)
		return 1;
	}
	if (_channels.find(name) != _channels.end())
	{
		std::cout<< "Channel already exists." << std::endl;
		return 1;
	}
	
	if (_channels.find(name) == _channels.end()) {
        std::cout << "Channel creation failed." << std::endl;
        return 1;
    }
	std::shared_ptr<Channel> newChannel = createChannel(name);
	
	std::string defaultTopic = "No topic set";
	newChannel->setTopic(defaultTopic, nullptr);

	Client* client = getClientByFd(fd);
	if (!client) {
        std::cout << "Client not found for fd: " << fd << std::endl;
        return 1;
    }
	if (newChannel->isUserInChannel(client)) {
        std::cout << "User is already in the channel." << std::endl;
        return 1;
    }
	newChannel->addUser(client);
	client->joinChannel(newChannel);
	newChannel->broadcastMessage(client->getNickname(), "has joined the channel");

	return  0;
}

int Serv::cmdPART(int fd, std::vector<std::string> line)
{
	if (line.empty())
	{
		std::cout<<"Not enough parameters for PART command"<< std::endl; // ERR_NEEDMOREARAMS(461)
		return 1;
	}
	if (line.size() > 2)
	{
		std::cout<< "Too many parameters for the PART command."<< std::endl;
		return 1;
	}
	std::string chanCheck = line[0];
	std::vector<std::string> channels;

	if (chanCheck.find(',') != std::string::npos)
	{
		channels = splitStr(chanCheck, ",");
		for (const std::string& channel : channels)
		{
			if (channel.empty() || channel[0] != '#') // Check if the channel is valid
        	{
            std::cout << "Invalid channel name: " << channel << std::endl;
            return 1; // Exit if any channel is invalid
        	}
		}
	}
	if (chanCheck[0] == '#')
	{
		channels.push_back(chanCheck);
	}
	else
	{
		std::cout<< "No channels found: incorrect input for PART."<< std::endl;
		return 1;
	}
	std::string reason;
	if (line.size() > 1)
	{
		reason = line[1];
	}
	for (const std::string& channelName : channels)
	{
		auto it = _channels.find(channelName);
		if (it == _channels.end())
		{
			std::cout<< "Channel "<< channelName<< " doesn't exist."<< std::endl;//ERR_NOSUCHCHANNEL(403)
			//return 1;
			continue ;
		}
		std::shared_ptr<Channel> channel = it->second;

		Client* client = getClientByFd(fd);
		if (!client) {
            std::cout << "Client not found for fd: " << fd << std::endl;
            return 1;
        }

		if (channel->isUserInChannel(client))
		{
			channel->removeUser(client);
			client->leaveChannel(channel);
		}
		else
		{
			std::cout<< "User is not in channel "<< channelName<< std::endl;//ERR_NOTONCHANNEL(442)
			//return 1;
			continue ;
		}
		std::string message = "left channel " + channelName;
		if (!reason.empty())
		{
			message += " (" + reason + ")";
			channel->broadcastMessage(client->getNickname(), message);
		}
		else{
			channel->broadcastMessage(client->getNickname(), message);
		}
	}
	return 0;
}

int Serv::cmdINVITE(int fd, std::vector<std::string> line)
{
	if (line.empty())
	{
		std::cout<<"Not enough parameters for INVITE command"<< std::endl; // ERR_NEEDMOREARAMS(461)
		return 1;
	}
	if (line.size() != 2)
	{
		std::cout<< "Too many parameters for the INVITE command."<< std::endl;
		return 1;
	}
	std::string newUser = line[0];
	std::string chanToAdd =line[1];
	if (chanToAdd[0] != '#')
	{
		std::cout<< "Invalid channel name."<<std::endl;
		return 1;
	}
	Client* client = getClientByFd(fd);
	if (!client) {
        std::cout << "Client not found for fd: " << fd << std::endl;
        return 1;
    }
	auto chanToFind = _channels.find(chanToAdd);
	if (chanToFind == _channels.end())
	{
		std::cout<< "Channel "<< chanToAdd<< " doesn't exist."<< std::endl;//ERR_NOSUCHCHANNEL(403)
		return 1;	
	}
	std::shared_ptr<Channel> channel = chanToFind->second;
	if (!channel->isUserInChannel(client))
	{
		std::cout<< "Client "<< client->getNickname()<< " not on channel."<<std::endl;// ERR_NOTONCHANNEL(403)
		return 1;
	}
	if (!channel->isOperator(client))
	{
		std::cout<<"Client is not an operator."<<std::endl;// ERR_CHANOPRIVSNEEDED(482)
		return 1;
	}
	Client* invitee = getClientByNickname(newUser);
	if (!invitee)
	{
		std::cout<< "User "<<newUser<< " not found."<< std::endl;
		return 1;
	}
	channel->addUser(invitee);
	invitee->joinChannel(channel);
	std::string message = "INVITE" + invitee->getNickname() + channel->getName();
	channel->broadcastMessage(client->getNickname(), message);
	return 0;
}

