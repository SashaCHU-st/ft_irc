/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 13:17:19 by alli              #+#    #+#             */
/*   Updated: 2025/02/14 12:05:08 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"

bool Serv::addNickname(int fd, std::string name)
{
	// if(name.size() == 0)
	// {
    //     std::cerr << "NO NICK!!!" << std::endl;
	// 	sendError(fd, "ERR_NONICKNAMEGIVEN: No nick name given",  431);
    //     return false;
	// }

	if (clients[fd].getFd() == fd)
	{
		// std::string nickname = tokens[0];
		if (clients[fd].getNickname().empty()) // new nickname if there's no nickname
		{
			// std::cout << "entered empty nickname" << std::endl;
			if (uniqueNickname(name) == true)
			{
				// std::cout << "true" << std::endl;
				clients[fd].setNickname(name);
				// std::string nick = " :ircserver 001 " + clients[fd].getNickname() + " added to network " + "\r\n";
				// if (send(fd, nick.c_str(), nick.size(), 0) == -1)
				// 	return false;
				return true;
			}
			else if (!uniqueNickname(name))
			{
				if (findLatestMatch(fd, name) > 0)
				{
					std::string latest = clients[findLatestMatch(fd, name)].getNickname();
					if (clients[fd].welcomeSent == false)
					{
						clients[fd].setNickname(latest + "_");
						if (findLatestMatch(fd, clients[fd].getNickname()) > 0)
						{
							// std::cout << "entered addNickname again" << clients[fd].getNickname() << std::endl;
							addNickname(fd, clients[fd].getNickname());
							
						}
						// std::string nick = " :ircserver 001 " + clients[fd].getNickname() + " added to network " + "\r\n";
						// if (send(fd, nick.c_str(), nick.size(), 0) == -1) 
						// 		return false;
						return true;
					}
					return false;
				}
				else
					return false;
			}
			else
				return false;
		}
		else if (uniqueNickname(name))
		{
			std::string oldName = clients[fd].getNickname();
			clients[fd].setNickname(name);
			std::string nick = ":" + oldName + " NICK " + clients[fd].getNickname() +  "\r\n";
			if (send(fd, nick.c_str(), nick.size(), 0) == -1) 
					return false;
			return true;
		}
		else if (!uniqueNickname(name))
		{
			if (findLatestMatch(fd, name) > 0)
			{
				std::string latest = clients[findLatestMatch(fd, name)].getNickname();
				if (clients[fd].welcomeSent == false)
				{
					clients[fd].setNickname(latest + "_");
					if (findLatestMatch(fd, clients[fd].getNickname()) > 0)
					{
						// std::cout << "2 entered addNickname again" << clients[fd].getNickname() << std::endl;
						addNickname(fd, clients[fd].getNickname());
					}
					// std::string nick = " :ircserver 001 " + clients[fd].getNickname() + " added to network " + "\r\n";
					// if (send(fd, nick.c_str(), nick.size(), 0) == -1) 
					// 		return false;
					return true;
				}
				return false;
			}
			else
				return false;
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
		// std::cout << "unique client.getnickname: " << client.getNickname() << std::endl;
        if (client.getNickname() == nickname)
        {
		   return false;
        }
        else
            continue;
    }
    return true;
}
int	Serv::findLatestMatch(int client_fd, std::string nickname)
{
	for (auto client = clients.rbegin(); client != clients.rend(); ++client)
	{
		if (client->second.getFd() == client_fd)
			continue;
		if (client->second.getNickname() == nickname)
			return client->first;
	}
	return -1;
}
	
