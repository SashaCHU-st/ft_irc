/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WelcomeMsg.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 12:16:25 by alli              #+#    #+#             */
/*   Updated: 2025/01/10 12:25:57 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"

void Serv::sendWelcomeMsg(int fd)
{
	if (!clients[fd].getUsername().empty() && !clients[fd].getNickname().empty())
	{
		clients[fd].allSet = true;
		std::string nick = " :ircserver 001 " + clients[fd].getNickname() + " :Welcome to network, " + clients[fd].getNickname() + "@localhost" + "\r\n";
		send(fd, nick.c_str(), nick.size(), 0);
			
		//welcome message
	}
}