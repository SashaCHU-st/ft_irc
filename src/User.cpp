/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 11:09:24 by alli              #+#    #+#             */
/*   Updated: 2025/01/09 13:31:59 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"

bool Serv::addUser(int client_fd, std::vector<std::string> tokens)
{ //username hostname servername realname
	int fd = findClient(client_fd);
	if (tokens.size() < 4)
	{
		std::string error_user = "Not enough parameters \r\n";
		return false;
	}
	if (fd > 0)
	{
		for(unsigned long i = 0; i < tokens.size(); i++)
		{
			std::cout << "token["<< i << "]: " << tokens[i] << std::endl;
		}
		if (tokens[0].length() > 12)
		{
			std::string newUser = tokens[0].substr(0, 12);
			clients[fd].setUsername(newUser);
		}
		else
			clients[fd].setUsername(tokens[0]);
		clients[fd].setHostName(tokens[1]);
		clients[fd].setServername(tokens[2]);
		if(tokens[3].find_first_of(":") == 0) //check ":" then rest of the name
		{
			for (unsigned long i = 3; i < tokens.size(); i++)
			{
				clients[fd].setRealname(tokens[i]);
			}
			return true;
		}
		else
			return false;
	}
	return false;
}