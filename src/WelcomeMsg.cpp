/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WelcomeMsg.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 12:16:25 by alli              #+#    #+#             */
/*   Updated: 2025/02/14 14:51:22 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"

void Serv::sendWelcomeMsg(int fd)
{

	
		// std::cout << "777" << std::endl;
	// std::cout << "FD"<<fd<< std::endl;
	// std::cout << "GetUsername => "<<clients[fd].getUsername() << "<="<< std::endl;
	// std::cout << "GetNicjname"<<clients[fd].getNickname()<< std::endl;
	
	if (!clients[fd].getUsername().empty() && !clients[fd].getNickname().empty())
	{
		if(clients[fd].welcomeSent == false)
		{
			// std::string nick = ":" + clients[fd].getServerName() + " 001 " + clients[fd].getNickname() + " :Welcome to network, " + clients[fd].getNickname() + "!" + clients[fd].getUsername() + "@" + clients[fd].getServerName() + "\r\n";
			// send(fd, nick.c_str(), nick.size(), 0);
			// std::string nick1 = ":" + clients[fd].getServerName() + " 002 " + clients[fd].getNickname() + " :Your host is " + clients[fd].getServerName( ) +  " running version 1.0 \r\n ";
			// send(fd, nick1.c_str(), nick1.size(), 0);
			std::string nick = ":" + clients[fd].getServerName() + " 001 " + clients[fd].getNickname() + " :Welcome to the network, " + clients[fd].getNickname() + "!" + clients[fd].getUsername() + "@" + clients[fd].getServerName() + "\r\n";
			std::cout << nick << std::endl;
			send(fd, nick.c_str(), nick.size(), 0);

			std::string nick1 = ":" + clients[fd].getServerName() + " 002 " + clients[fd].getNickname() + " :Your host is " + clients[fd].getServerName() + ", running version 1.0\r\n";  // 🚀 FIXED: Removed extra space before \r\n
			send(fd, nick1.c_str(), nick1.size(), 0);

			std::string nick2 = ":" + clients[fd].getServerName() + " 003 " + clients[fd].getNickname() + " :This server was created today\r\n ";
			send(fd, nick2.c_str(), nick2.size(), 0);
			std::string nick3 = ":" + clients[fd].getServerName() + " 004 " + clients[fd].getNickname() + " :localhost 1.0 iow\r\n " + clients[fd].getNickname() + "@localhost" + "\r\n";
			send(fd, nick3.c_str(), nick3.size(), 0);
			
				
			//welcome message
			clients[fd].welcomeSent = true;
			clients[fd].allSet = true;
			std::cout << "finished printing welcome message" << std::endl;
		}
	}
}

