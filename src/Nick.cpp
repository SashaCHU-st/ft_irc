/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 13:17:19 by alli              #+#    #+#             */
/*   Updated: 2025/01/14 11:08:07 by alli             ###   ########.fr       */
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
	if (clients[fd].getFd() == fd)
	{
		std::string nickname = tokens[0];
		if (clients[fd].getNickname().empty()) // new nickname if there's no nickname
		{
			std::cout << "entered empty nickname" << std::endl;
			if (uniqueNickname(nickname) == true)
			{
				std::cout << "entered empty nickname" << std::endl;
				clients[fd].setNickname(nickname);
				std::string nick = " :ircserver 001 " + clients[fd].getNickname() + " added to network " + "\r\n";
				if (send(fd, nick.c_str(), nick.size(), 0) == -1)
					return false;
				return true;
			}
			else if (!uniqueNickname(nickname))
			{
				if (clients[fd].welcomeSent == false)
				{
					std::cout << "fd: " << fd << std::endl;
					clients[fd].setNickname(nickname + "_");
					std::string nick = " :ircserver 001 " + clients[fd].getNickname() + " added to network " + "\r\n";
					if (send(fd, nick.c_str(), nick.size(), 0) == -1) 
							return false;
					return true;
				}
				return false;
			}
			else
				return false;
		}
		else if (uniqueNickname(nickname))
		{
			std::cout << "unique nickname here" << std::endl;
			std::string oldName = clients[fd].getNickname();
			std::cout << oldName << std::endl;
			clients[fd].setNickname(nickname);
			std::string nick = ":" + oldName + " NICK " + clients[fd].getNickname() +  "\r\n";
			if (send(fd, nick.c_str(), nick.size(), 0) == -1) 
					return false;
			return true;
		}
		else 
			return false;
	}
    return false;
}

bool Serv::uniqueNickname(std::string nickname)
{
    // std::cout << "client size in unique nickname" << clients.size() << std::endl;
    for(auto& [fd, client] : clients)
    {
		std::cout << "unique client.getnickname: " << client.getNickname() << std::endl;
        if (client.getNickname() == nickname)
        {
		   return false;
        }
        else
            continue;
    }
    return true;
}