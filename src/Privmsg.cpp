/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 14:10:11 by alli              #+#    #+#             */
/*   Updated: 2025/01/14 15:28:46 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"
#include "Channel.hpp"

bool Serv::message(int client_fd, std::vector<std::string> tokens)
{
	std::string message;
	int msg_fd = findLatestMatch(tokens[0])
	
	if (clients[client_fd].getNickname() == tokens[0])
	{
		
		for(unsigned long i = 1; i < tokens.size(); i++)
		{
			message += tokens[i] + " "; 
		}
		send(clients[msg_fd].getFd(), message.c_str(), message.size(), 0);
	}
	if (_channels.find(tokens[0]) != _channels.end())
	{
		std::cout << "work" << std::endl;
	}
}