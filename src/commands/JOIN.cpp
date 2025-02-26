/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JOIN.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: epolkhov <epolkhov@student.42.fr>          #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-02-24 17:14:51 by epolkhov          #+#    #+#             */
/*   Updated: 2025-02-24 17:14:51 by epolkhov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Serv.hpp"

int Serv::cmdJOIN(int fd, std::vector<std::string> line)
{
	if (line.empty())
	{
		//std::cout<<"No parameter after JOIN"<< std::endl;
		//sendError(fd, "ERR_NEEDMOREPARAMS" , 461);
		sendError(fd, "JOIN :Not enough parameters", 461);
		return 1;
	}
	if (line.size() > 2)
	{
		//std::cerr<< "Too many parameters for the JOIN command."<< std::endl;
		//sendError(fd, "ERR_TOOMANYPARAMS" , 461);
		sendError(fd, "JOIN :Too many parameters", 461);
		return 1;
	}
	std::string chanCheck = line[0];
	std::string keyCheck;
	if (line.size() > 1)
	{
		keyCheck = line[1];
	}
	else{
		keyCheck = "";
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
	for (size_t i = 0; keys.size() > i; i++)
	{
		std::cout << "Keys array: "<< keys[i]<< std::endl;
	}
	Client* client = getClientByFd(fd);
	if (!client || client->allSet == false) {
        //std::cerr << "Client not found for fd: " << fd << std::endl;
		sendError(fd, "ERR_NOSUCHCLIENT", 4);
        return 1;
    }
	if (channels.size() != keys.size() && keys.size() != 1) {
        //std::cerr << "Number of keys doesn't match the number of channels." << std::endl;
		sendError(fd, "ERR_BADCHANNELKEY: Number of keys doesn't match the number of channels",  475);
        return 1;
    }
	int count_joined = 0;
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (count_joined > 5)
		{
			sendError(fd, "ERR_TOOMANYCHANNELS", 405);
			return 1;
		}
		const std::string& chan = channels[i];
		if (chan.empty() || chan[0] != '#' || checkChanName(chan) == 1)
		{
			//std::cerr<< "Invalid channel name: "<< chan<< ": it should start with #"<<std::endl;
			sendError(fd, "ERR_NOSUCHCHANNEL", 403);
			continue ;
		}
		auto findChan = _channels.find(chan);
		std::shared_ptr<Channel> newChannel;
		if (findChan == _channels.end())
		{
			newChannel = createChannel(chan);
			std::string defaultTopic = "";
			newChannel->setTopic(defaultTopic, nullptr);
			newChannel->addUser(client);
			client->joinChannel(newChannel);
			std::string modeMessage = ":" + client->getServerName() +
				" MODE " + newChannel->getName() +
				" +o " + client->getNickname() + "\r\n";
			std::string userList = "@" + newChannel->getOperator(client)->getNickname() + ""
					+ " " + newChannel->getUsersNick();
			std::string msgJoin = ":" + client->getNickname() + "!" + client->getUsername() + "@" 
					+ client->getServerName()
					+ " JOIN " + newChannel->getName() + "\r\n"; 
			std::string msg353 =  ":" + client->getServerName() + " 353 " + client->getNickname()
					+ " = " + newChannel->getName() + " :" + userList + "\r\n";
		
			//std::cout << userList << std::endl;
			newChannel->sendToAll(modeMessage);
			newChannel->sendToAll(msgJoin);
			newChannel->sendToAll(msg353);
			std::string msg366 = ":" + client->getServerName() + " 366 " + client->getNickname() 
								+ " " + newChannel->getName() + "\r\n";
			newChannel->sendToAll(msg366);
			count_joined++;
		}
		else
		{
			//std::cout<< "Channel: "<< chan <<" already exists." << std::endl;
			newChannel = findChan->second;
			std::cout<< "User limit"<< newChannel->getUserLimit()<< std::endl;
			if (!newChannel->getPassword().empty())
			{
				if (keys[i].empty())
				{
					//sendError(fd, "ERR_BADCHANNELKEY" + client->getNickname() + " " + newChannel->getName() + " :Cannot join channel (+k) ", 475);
					sendError(fd, newChannel->getName() + " :Cannot join channel (+k)", 475);
					continue ;
				}
				if (!keys[i].empty() && !newChannel->checkPassword(keys[i]))
				{
					//std::cerr << "Incorrect password for channel: " << chan << std::endl;
					//sendError(fd, "ERR_BADCHANNELKEY" + client->getNickname() + " " + newChannel->getName() + " :Incorrect password (+k) ", 475);
					sendError(fd, newChannel->getName() + " :Cannot join channel (+k)", 475);
					continue ;
				}
			}
			if (newChannel->isUserInChannel(client)) {
				//std::cout << "User " << client<< " is already in the channel: "<< newChannel->getName() << std::endl;
				//sendError(fd, "ERR_USER_ALREADY_IN_CHANNEL " + client->getNickname() + " " + newChannel->getName() + " :You are already in the channel", 433);
				sendError(fd, newChannel->getName() + " :You are already in the channel", 433);
				continue ;
			}
			if (newChannel->getUserLimit() > 0)
			{
				if (newChannel->getUserLimit() <= newChannel->getUserCount())
				{
					//std::cout<< "User "<< client->getNickname() << " cannot join the channel as the ammount of users are limited."<< std::endl;
					//sendError(fd, "ERR_CHANNELISFULL" + client->getNickname() + " " + newChannel->getName() + " :Cannot join channel (+l)", 471);
					sendError(fd, newChannel->getName() + " :Cannot join channel (+l)", 471);
					continue ;
				}
			}
			if (newChannel->isInviteOnly())
			{
				if (client->isInvitedToChan() == false)
				{
					//std::cout<< "User "<< client->getNickname()<< " tries to join the channel and wait for INVITE." << std::endl;
					//sendError(fd, "ERR_INVITEONLYCHAN " + newChannel->getName() + " :Cannot join channel (+i)", 473);
					sendError(fd, newChannel->getName() + " :Cannot join channel (+i)", 473);
					continue ;
				}
			}		
			newChannel->addUser(client);
			client->joinChannel(newChannel);
			if (newChannel->isOperator(client) == false)
			{
				std::string msg353 = ":" + client->getServerName() + " 353 " + client->getNickname() 
								+ " = " + newChannel->getName() + " :" + newChannel->getUsersNick() + "\r\n";
				//std::cout << "msg353 3:" << msg353 << std::endl;
				std::string msgJoin = ":" + client->getNickname() + "!" + client->getUsername() + "@" 
					+ client->getServerName()
					+ " JOIN " + newChannel->getName() + "\r\n"; 
				std::cout << "msgJoin2: " << msgJoin << std::endl;
				newChannel->sendToAll(msgJoin);
				newChannel->sendToAll(msg353);
			}
			else //if (newChannel->isOperator(client) == true)
			{
				std::string modeMessage = ":" + client->getServerName() +
					" MODE " + newChannel->getName() +
					" +o " + client->getNickname() + "\r\n";
				std::string msg353 = ":" + client->getServerName() + " 353 " + client->getNickname()
					+ newChannel->getName() + " @" + newChannel->getOperator(client)->getNickname()
					+ newChannel->getUsersNick() + "\r\n";
				//std::cout << "msg353 4:" << msg353 << std::endl;
				std::string msgJoin = ":" + client->getNickname() + "!" + client->getUsername() + "@" 
					+ client->getServerName() + " JOIN " + newChannel->getName() + "\r\n"; 
				std::cout << "msgJoin3: " << msgJoin << std::endl;
				newChannel->sendToAll(msgJoin);
				newChannel->sendToAll(msg353);
			}
			std::string msg366 = ":" + client->getServerName() + " 366 " + client->getNickname() 
				+ " " + newChannel->getName() + "\r\n";
			newChannel->sendToAll(msg366);
			std::string topic = newChannel->getTopic();
			if (!topic.empty())
			{
				std::string topicMessage = ":ircserver 332 " + client->getNickname() + " " + newChannel->getName() + " :Welcome to " + topic + "\r\n";
                std::cout << topicMessage << std::endl;
				send(fd, topicMessage.c_str(), topicMessage.size(), 0);
			}
			count_joined++;
			
		}
	}
	return  0;
}