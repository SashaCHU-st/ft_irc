/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PART.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: epolkhov <epolkhov@student.42.fr>          #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-02-24 17:21:50 by epolkhov          #+#    #+#             */
/*   Updated: 2025-02-24 17:21:50 by epolkhov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Serv.hpp"

int Serv::cmdPART(int fd, std::vector<std::string> line)
{
	if (line.empty())
	{
		//std::cerr << "Need more params" << std::endl;
		sendError(fd, "PART :Need more params",  461);
		return 1;
	}
	std::string chanCheck = line[0];
	std::vector<std::string> channels;

	if (chanCheck.find(',') != std::string::npos)
	{
		channels = splitStr(chanCheck, ",");
		for (const std::string& channel : channels)
		{
			if (channel.empty() || channel[0] != '#' || checkChanName(channel) == 1)
        	{
            	//std::cout << "Invalid channel name: " << channel << std::endl;
				sendError(fd, channel + " :No such channel", 403);
            	//return 1;
				continue ;
        	}
		}
	}
	if (chanCheck[0] == '#' && checkChanName(chanCheck) != 1)
	{
		channels.push_back(chanCheck);
	}
	else
	{
		//std::cerr << "No such channel" << std::endl;
		sendError(fd, "ERR_NOSUCHCHANNEL :No such channel",  403);
		return 1;
	}
	std::string reason;
	if (line.size() > 1)
	{
		reason = "";
    	for (size_t i = 1; i < line.size(); ++i)
		{
        	reason += line[i];
        	if (i != line.size() - 1) 
			{
           		reason += " ";
        	}
    	}
	}
	for (const std::string& channelName : channels)
	{
		auto it = _channels.find(channelName);
		if (it == _channels.end())
		{
			//std::cerr << "No such channel" << std::endl;
			sendError(fd, "ERR_NOSUCHCHANNEL :No such channel",  403);
			continue;
		}
		std::shared_ptr<Channel> channel = it->second;

		Client* client = getClientByFd(fd);
		if (!client) {
            //std::cout << "Client not found by fd: " << fd << std::endl;
			sendError(fd, "ERR_NOSUCHCLIENT", 4);
            return 1;
        }

		if (channel->isUserInChannel(client))
		{
			if (channel->isOperator(client))
			{
				channel->removeUser(client);
				client->leaveChannel(channel->getName());
				if (!channel->getUsers().empty())
            	{
               	 	std::vector<Client*> usersInChannel = channel->getUsers();
                	srand(time(0));
                	int randomIndex = rand() % usersInChannel.size();
               		Client* randomUser = usersInChannel[randomIndex];
                	channel->addOperator(randomUser);
					std::string modeMessage = ":" + client->getServerName() + " MODE " + channelName +
                                          " +o " + randomUser->getNickname() + "\r\n";
                	channel->sendToAll(modeMessage);
                	//std::cout << "Assigned user " << randomUser->getNickname() << " as operator in channel " << channelName << std::endl;
            	}
			}
			else{
				channel->removeUser(client);
				client->leaveChannel(channel->getName());
			}
		}
		else
		{
			//std::cerr << "No channel" << std::endl;
			sendError(fd, "ERR_NOTONCHANNEL :No channel",  442);
        	continue;
		}
		if (!reason.empty()) {
			std::string message = ":" + client->getNickname() + "!" + client->getUsername() + "@"
				+ client->getServerName() + " PART " + channelName + " :" + reason + "\r\n";
			send_message(client->getFd(), message);
			channel->sendToAll(message);
		} else {
			std::string message = ":" + client->getNickname() + "!" + client->getUsername() + "@"
				+ client->getServerName() + " PART " + channelName + "\r\n";
			send_message(client->getFd(), message);
			channel->sendToAll(message);
		}
	}
	return 0;
}