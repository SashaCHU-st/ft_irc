/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 13:17:19 by alli              #+#    #+#             */
/*   Updated: 2025/01/13 16:01:54 by alli             ###   ########.fr       */
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
				std::cout << "entered empty nickname" << std::endl;
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
            else if (uniqueNickname(nickname) == true || clients.size() == 1) //replacing nickname
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
            else if (uniqueNickname(nickname) == false && clients[fd].welcomeSent == false)
            {
				std::cout << "entered add nickname, false and welcome false" << std::endl;
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
		std::cout << "in unique nickname: " << nickname << std::endl;
		std::cout << "client.getnickname: " << client.getNickname() << std::endl;
        if (client.getNickname() == nickname)
        {
           if(client.welcomeSent == false || clients.size() > 1)
		   {
				std::cout << "entered unique nickname welcome sent false" << std::endl;
				client.setNickname(nickname + "_");
				return true;
		   }
		   return false;
        }
        else
            continue;
    }
    return true;
}