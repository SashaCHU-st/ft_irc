/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 14:10:11 by alli              #+#    #+#             */
/*   Updated: 2025/02/13 11:05:43 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"
#include "Channel.hpp"

bool Serv::message(int client_fd, std::vector<std::string> tokens)
{
	std::string message;
	int msg_fd = findLatestMatch(-1, tokens[0]);
	int receiver_fd = clients[msg_fd].getFd();
	
	if (clients.find(msg_fd) == clients.end())
	{
		std::cout << "Client not found" << std::endl;
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
	return false;
}