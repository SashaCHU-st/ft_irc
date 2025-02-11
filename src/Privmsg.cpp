/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 14:10:11 by alli              #+#    #+#             */
/*   Updated: 2025/01/15 11:24:17 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"
#include "Channel.hpp"

bool Serv::message(int client_fd, std::vector<std::string> tokens)
{
	std::string message;
	int msg_fd = findLatestMatch(-1, tokens[0]);
	int receiver_fd = clients[msg_fd].getFd();
	(void)client_fd;
	
	std::cout << "client fd: " << client_fd << std::endl;
	if (clients[msg_fd].getNickname() == tokens[0])
	{
		for(unsigned long i = 1; i < tokens.size(); i++)
		{
			if (tokens[1][0] == ':')
			{
				tokens[1] = tokens[1].substr(1);
				message += " " + tokens[1] + " ";
			}
			else
				message += tokens[i] + " "; 
		}
		std::string msg = clients[client_fd].getNickname() + "!" + clients[client_fd].getUsername() + "@" + clients[client_fd].getServerName()
			+ " PRIVMSG " + clients[msg_fd].getNickname() + message + "\r\n";
		if (send(receiver_fd, msg.c_str(), msg.size(), 0) == -1)
		{
			std::cerr << "message failed to send" << std::endl;
			return false;
		}
		std::cout << "message sent" << std::endl;
		return true;
	}
	if (_channels.find(tokens[0]) != _channels.end())
	{
		std::cout << "work" << std::endl;
		return true;
	}
	return false;
}