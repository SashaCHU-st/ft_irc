/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 11:09:24 by alli              #+#    #+#             */
/*   Updated: 2025/02/21 12:38:34 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"

bool Serv::addUser(int client_fd, std::vector<std::string> tokens)
{
	if (tokens.size() < 4)
	{
		std::string error_user = "Not enough parameters \r\n";
		return false;
	}
	if (clients[client_fd].getUsername().empty())
	{
		if (tokens[0].length() > 12)
		{
			std::string newUser = tokens[0].substr(0, 12);
			clients[client_fd].setUsername(newUser);
		}
		else
		{
			clients[client_fd].setUsername(tokens[0]);
		}
		clients[client_fd].setHostName(tokens[1]);
		clients[client_fd].setServername(tokens[2]);
		if(tokens[3].find_first_of(":") == 0) //check ":" then rest of the name
		{
			for (unsigned long i = 3; i < tokens.size(); i++)
			{
				clients[client_fd].setRealname(tokens[i]);
			}
			clients[client_fd].allSet = true;
			return true;
		}
	}
	else
	{
		if (tokens[0].length() > 12)
		{
			std::string newUser = tokens[0].substr(0, 12);
			clients[client_fd].setUsername(newUser);
		}
		else
		{
			clients[client_fd].setUsername(tokens[0]);
		}
		clients[client_fd].setHostName(tokens[1]);
		clients[client_fd].setServername(tokens[2]);
		if(tokens[3].find_first_of(":") == 0) //check ":" then rest of the name
		{
			for (unsigned long i = 3; i < tokens.size(); i++)
			{
				clients[client_fd].setRealname(tokens[i]);
			}
			clients[client_fd].allSet = true;
			return true;
		}
	}
	return false;
}