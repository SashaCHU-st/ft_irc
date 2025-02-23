/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 13:17:19 by alli              #+#    #+#             */
/*   Updated: 2025/02/21 08:38:06 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"

bool Serv::addNickname(int fd, std::string name)
{
	if (clients[fd].getFd() == fd)
	{
		if (clients[fd].getNickname().empty())
		{
			if (uniqueNickname(name) == true)
			{
				clients[fd].setNickname(name);
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
							addNickname(fd, clients[fd].getNickname());
						}
						return true;
					}
					sendError(fd, "ERR_NICKNAMEINUSE", 433);
					return false;
				}
				else
					return false;
			}
			else
			{
				sendError(fd, "ERR_NICKNAMEINUSE", 433);
				return false;
			}
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
						addNickname(fd, clients[fd].getNickname());
					}
					return true;
				}
				sendError(fd, "ERR_NICKNAMEINUSE", 433);
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
    for(auto& [fd, client] : clients)
    {
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
	
