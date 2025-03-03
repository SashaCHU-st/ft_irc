/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 14:10:11 by alli              #+#    #+#             */
/*   Updated: 2025/02/25 09:58:24 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"
#include "Channel.hpp"

bool Serv::message(int client_fd, std::vector<std::string> tokens)
{
	std::string message;
	int msg_fd = findLatestMatch(-1, tokens[0]);
	int receiver_fd = clients[msg_fd].getFd();
	
	if (msg_fd == -1 || receiver_fd == -1)
		return false;
	if (clients.find(msg_fd) == clients.end()) //checking clients
	{
		std::cout << "Client not found" << std::endl;
		sendError(client_fd, "ERR_NOSUCHNICK", 401);
		return false;	
	}
	if (clients[msg_fd].getNickname() == tokens[0])
	{
		for(unsigned long i = 1; i < tokens.size(); i++)
		{
			message += tokens[i] + " "; 
		}
		std::string msg = ":" + clients[client_fd].getNickname() + "!" + clients[client_fd].getUsername() + "@" + clients[client_fd].getServerName()
			+ " PRIVMSG " + clients[msg_fd].getNickname() + " " + message + "\r\n"; 
		if (send(receiver_fd, msg.c_str(), msg.size(), 0) == -1)
		{
			std::cerr << "message failed to send" << std::endl;
			return false;
		}
		std::cout << "message sent" << std::endl;
		return true;
	}
	auto chanToFind = _channels.find(tokens[0]); //checking channels
	if (chanToFind == _channels.end())
	{
		sendError(client_fd, "ERR_NOSUCHCHANNEL", 403);
		std::cout << "no channel found" << std::endl;
	}
	else
	{
		std::vector userList = chanToFind->second->getUsers();
		if (userList.empty())
		{
			std::cout << "No users in group" << std::endl;
			return false;
		}
		//check if they are part of the group
		for (size_t i = 0; i < userList.size(); i++)
		{
			Client* user = userList[i];
			if (user->getFd() == client_fd)
				break;
			else if (user->getFd() != client_fd)
			{
				if (i == userList.size() - 1)
					return false;
				continue;
			}
			else
				return false;
		}
		for(unsigned long i = 1; i < tokens.size(); i++)
		{
			message += tokens[i] + " "; 
		}
		for (size_t i = 0; i < userList.size(); i++)
		{
			int tmpFd = userList[i]->getFd();
			if (client_fd == tmpFd)
				continue;
			if (clients[client_fd].getUsername().empty() || clients[client_fd].getServerName().empty() || clients[client_fd].getNickname().empty())
			{
				sendError(client_fd, "ERR_NOSUCHCHANNEL", 403);
				continue;
			}
 			std::string msg = ":" + clients[client_fd].getNickname() + "!" + clients[client_fd].getUsername() + "@" + clients[client_fd].getServerName()
				+ " PRIVMSG " + tokens[0] + " " + message + "\r\n";
			if (send(tmpFd, msg.c_str(), msg.size(), 0) == -1)
			{
				std::cerr << "message failed to send" << std::endl;
				return false;
			}
		}
	}
	return false;
}