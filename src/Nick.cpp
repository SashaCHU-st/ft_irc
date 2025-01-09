/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 13:17:19 by alli              #+#    #+#             */
/*   Updated: 2025/01/09 10:47:30 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"

bool Serv::addNickname(int fd, std::vector<std::string> tokens)
{
	// std::cout << "nickname token: " << tokens[0] << std::endl;
	if (tokens.size() > 1)
	{
		std::string error_nick = std::string("please only input 1 nickname ") + "\r\n";
		send(fd, error_nick.c_str(), error_nick.size(), 0);
	}
	for (unsigned long i = 0; i < clients.size(); i++)
	{
		int tmpFd = clients[i].getFd();
		if (tmpFd == fd)
		{
			std::string nickname = tokens[0];
			std::cout << "fd in addnickname" << fd << std::endl;
			if (clients[fd].getNickname().empty()) // new nickname if there's no nickname
			{
				if (uniqueNickname(nickname) == true)
				{
					clients[fd].setNickname(nickname);
					std::cout << "empty nickname " << clients[fd].getNickname() << std::endl;
					return true;
				}
				else
					return false;
			}
			if (uniqueNickname(nickname) == true) //replacing nickname
			{
				clients[fd].setNickname(nickname);
				std::cout << "Client nickname: " << clients[fd].getNickname() << std::endl;
				return true;
			}
			else
				return false;
		}
	}
	return false;
}

bool Serv::uniqueNickname(const std::string& nickname)
{
	for (unsigned long i = 0; i < clients.size(); i++)
	{
		std::string tmpnick = clients[i].getNickname();
		if (tmpnick == nickname)
		{
			std::cerr << "nickname is taken, please choose another one" << std::endl;
			return false;
		}
		else
			continue;
	}
	return true;
}