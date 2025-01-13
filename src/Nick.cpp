/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 13:17:19 by alli              #+#    #+#             */
/*   Updated: 2025/01/13 11:13:19 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"

bool Serv::addNickname(int fd, std::vector<std::string> tokens)
{
	if (tokens.size() > 1)
	{
		std::string error_nick = std::string("please only input 1 nickname ") + "\r\n";
		send(fd, error_nick.c_str(), error_nick.size(), 0);
	}
	// for (unsigned long i = 0; i < clients.size(); i++)
	if (clients[fd].getFd() == fd)
	{
			std::string nickname = tokens[0];
			// std::cout << "fd in addnickname" << fd << std::endl;
			if (clients[fd].getNickname().empty()) // new nickname if there's no nickname
			{
				if (uniqueNickname(nickname) == true)
				{
					clients[fd].setNickname(nickname);
					std::string nick = " :ircserver 001 " + clients[fd].getNickname() + " added to network " + "\r\n";
					if (send(fd, nick.c_str(), nick.size(), 0) == -1)
						return false;
					return true;
				}
				else
					return false;
			}
			if (uniqueNickname(nickname) == true) //replacing nickname
			{
				std::cout << "unique nickname here" << std::endl;
				std::string oldName = clients[fd].getNickname();
				std::cout << oldName << std::endl;
				clients[fd].setNickname(nickname);
				std::string nick = oldName + " changed their nickname to " + clients[fd].getNickname() +  "\r\n";
				if (send(fd, nick.c_str(), nick.size(), 0) == -1)
						return false;
				return true;
			}
			else
			{
				return false;
			}
		}
	return false;
}

bool Serv::uniqueNickname(std::string nickname)
{
	std::cout << "client size in unique nickname" << clients.size() << std::endl;
	for(const auto& [fd, client] : clients)
	{
		if (client.getNickname() == nickname)
		{
			std::cerr << "nickname is taken, please choose another one" << std::endl;
			return false;
		}
		else
			continue;
	}
	return true;
}