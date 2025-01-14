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

int Serv::cmdJOIN(int fd, std::vector<std::string> line)
{
	if (line.empty())
	{
		std::cout<<"No parameter after JOIN"<< std::endl;// ERR_NEEDMOREPARAMS (461)
		return 1;
	}
	if (line.size() > 2)
	{
		std::cout<< "Too many parameters for the PART command."<< std::endl;
		return 1;
	}
	std::string chanCheck = line[0];
	std::string keyCheck = "";
	if (line.size() > 1)
	{
		keyCheck = line[1];
	}
	std::vector<std::string> channels;
	std::vector<std::string> keys;
	if (chanCheck.find(',') != std::string::npos)
	{
		channels = splitStr(chanCheck, ",");
	}
	else{
		channels.push_back(chanCheck);
	}
	if (keyCheck.find(',') != std::string::npos)
	{
		keys = splitStr(keyCheck, ",");
	}
	else{
		keys.push_back(keyCheck);
	}
	Client* client = getClientByFd(fd);
	if (!client) {
        std::cout << "Client not found for fd: " << fd << std::endl;
        return 1;
    }
	for (size_t i = 0; i < channels.size(); ++i)
	{
		const std::string& chan = channels[i];
		if (chan.empty() || chan[0] != '#')
		{
			std::cout<< "Invalid channel name: "<< chan<< ": it should start with #"<<std::endl;
			continue ;
		}
		auto findChan = _channels.find(chan);
		std::shared_ptr<Channel> newChannel;
		if (findChan == _channels.end())
		{
			newChannel = createChannel(chan);
			std::string defaultTopic = "No topic set";
			newChannel->setTopic(defaultTopic, nullptr);
			if (i < keys.size() && !keys.empty())
			{
				newChannel->setPassword(keys[i]);
				std::cout << "Password set for channel " << chan << ": " << keys[i] << std::endl;
			}
		}
		else
		{
			std::cout<< "Channel: "<< chan <<" already exists." << std::endl;
			newChannel = findChan->second;
			if (!newChannel->getPassword().empty())
			{
				if (i < keys.size() && !keys[i].empty() && !newChannel->checkPassword(keys[i]))
				{
					std::cout << "Incorrect password for channel: " << chan << std::endl;
            		continue;
				}
			}
			if (newChannel->isUserInChannel(client)) {
				std::cout << "User " << client<< " is already in the channel: "<< newChannel->getName() << std::endl;
				continue ;
			}
		}
		newChannel->addUser(client);
		client->joinChannel(newChannel);
		newChannel->broadcastMessage(client->getNickname(), "has joined the channel");
	}
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
            return 1;
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
            std::cout << "Client not found by fd: " << fd << std::endl;
            return 1;
        }

		if (channel->isUserInChannel(client) || channel->isOperator(client))
		{
			if (channel->isOperator(client))
			{
				channel->removeUser(client);
				client->leaveChannel(channel);
				if (!channel->getUsers().empty()) // Check if the channel still has users
            	{
               	 	std::vector<Client*> usersInChannel = channel->getUsers();
                	srand(time(0)); // Seed the random number generator
                	int randomIndex = rand() % usersInChannel.size(); // Random index in range
               		Client* randomUser = usersInChannel[randomIndex];
                	channel->addOperator(randomUser);
                	std::cout << "Assigned user " << randomUser->getNickname() << " as operator in channel " << channelName << std::endl;
            	}
			}
			else{
				channel->removeUser(client);
				client->leaveChannel(channel);
			}
		}
		else
		{
			std::cout<< "User "<< client->getNickname() <<" is not in channel "<< channelName<< std::endl;//ERR_NOTONCHANNEL(442)
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
	if (chanToAdd[0] != '#' || chanToAdd.empty())
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
	if (channel->isUserInChannel(invitee)) {
        std::cout << "User " << newUser << " is already in channel " << chanToAdd << "." << std::endl;//ERR_USERONCHANNEL(443)
        return 1;
    }
	channel->addUser(invitee);
	invitee->joinChannel(channel);
	std::string message = invitee->getServerName() + "INVITE " + invitee->getNickname() + " :" + channel->getName();
	send_message(invitee->getFd(), message);
	std::cout << "User " << newUser << " successfully invited to channel " << chanToAdd << "." << std::endl;
	return 0;
}

