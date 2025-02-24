/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   KICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: epolkhov <epolkhov@student.42.fr>          #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-02-24 17:26:49 by epolkhov          #+#    #+#             */
/*   Updated: 2025-02-24 17:26:49 by epolkhov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Serv.hpp"

int Serv::cmdKICK(int fd, std::vector<std::string> line)
{
    std::cout << "Line size: " << line.size() << std::endl;
    if (line.empty() || line.size() < 2)
    {
		sendError(fd, "KICK :Not enough parameters", 461);
        return 1;
    }

    std::string checkChan = line[0];
    if (checkChan[0] != '#' || checkChanName(checkChan) == 1)
    {
		sendError(fd, checkChan + " :Channel call is incorrect", 403);
        return 1;
    }

    auto findChan = _channels.find(checkChan);
    if (findChan == _channels.end())
    {
		sendError(fd, checkChan + " :No such channel", 403);
        return 1;
    }

    std::shared_ptr<Channel> channel = findChan->second;
    Client* client = getClientByFd(fd);
    if (!client)
	{
		sendError(fd, checkChan + " :You're not on that channel", 442);
        return 1;
    }
	if (!channel->isOperator(client))
    {
        sendError(fd, checkChan + " :You're not a channel operator", 482);
        return 1;
    }

    std::string reason;
	size_t reasonIndx = line.size();
    std::string message = "";

	for (size_t i = 1; i < line.size(); ++i)
	{
		if (line[i][0] == ':')
		{
			reasonIndx = i;
			break;	
		}
	}
	if (reasonIndx < line.size())
	{
    	reason = line[reasonIndx].substr(1);
    	for (size_t i = reasonIndx + 1; i < line.size(); ++i)
   		{
        	reason += " " + line[i]; 
    	}
	}
		
    for (size_t i = 1; i < reasonIndx; ++i)
    {
		Client* userKick = getClientByNickname(line[i]);
        if (!userKick || !channel->isUserInChannel(userKick))
        {
            //sendError(fd, "ERR_USERNOTINCHANNEL", 441);
			std::cout << "Is it not user in channel?"<< std::endl;
			sendError(fd, line[i] + " " + checkChan + " :User is not on that channel", 441);
            continue;
        }
		std::string kickMessage = ":" + client->getServerName() + " KICK " 
                    + channel->getName() + " " + userKick->getNickname() + "\r\n";

		send(userKick->getFd(), kickMessage.c_str(), kickMessage.size(), 0);

		message = ":" + client->getNickname() + "!" + client->getUsername() + "@" 
        			+ client->getServerName() + " KICK " + channel->getName() + " " + userKick->getNickname();
		if (!reason.empty())
		{
			message += " :" + reason;
		}
		message += "\r\n";
		channel->sendToAll(message);  
        channel->removeUser(userKick);
        userKick->leaveChannel(channel->getName());
		//std::cout << "KICK message sent to client " << userKick->getNickname() << ": " << kickMessage << std::endl;
    }
    return 0;
}