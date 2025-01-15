/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 14:10:11 by alli              #+#    #+#             */
/*   Updated: 2025/01/15 10:28:08 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"
#include "Channel.hpp"

bool Serv::message(int client_fd, std::vector<std::string> tokens)
{
	std::string message;
	int msg_fd = findLatestMatch(-1, tokens[0]);
	(void)client_fd;
	
	// std::cout << "Sending client: " << clients[msg_fd].getNickname() << std::endl;
	// std::cout << "token client: " << tokens[0] << std::endl;
	if (clients[msg_fd].getNickname() == tokens[0])
	{
		for(unsigned long i = 1; i < tokens.size(); i++)
		{
			if (tokens[1][0] == ':')
			{
				tokens[1] = tokens[1].substr()
			}
			
			message += tokens[i] + " "; 
		}
		std::cout << "message: " << message << std::endl;
		if (send(msg_fd, message.c_str(), message.size(), 0) == -1)
		{
			std::cerr << "message failed to send" << std::endl;
			return false;
		}
		return true;
	}
	if (_channels.find(tokens[0]) != _channels.end())
	{
		std::cout << "work" << std::endl;
		return true;
	}
	return false;
}