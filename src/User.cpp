/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 11:09:24 by alli              #+#    #+#             */
/*   Updated: 2025/01/14 11:08:21 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"

bool Serv::addUser(int client_fd, std::vector<std::string> tokens)
{ //username hostname servername realname
	// std::cout << "entered add user" << std::endl;
	if (tokens.size() < 4)
	{
		std::string error_user = "Not enough parameters \r\n";
		return false;
	}
	if (clients[client_fd].getUsername().empty())
	{
		// for(unsigned long i = 0; i < tokens.size(); i++)
		// {
		// 	std::cout << "token["<< i << "]: " << tokens[i] << std::endl;
		// }
		if (tokens[0].length() > 12)
		{
			std::string newUser = tokens[0].substr(0, 12);
			clients[client_fd].setUsername(newUser);
		}
		else
			clients[client_fd].setUsername(tokens[0]);
		clients[client_fd].setHostName(tokens[1]);
		clients[client_fd].setServername(tokens[2]);
		if(tokens[3].find_first_of(":") == 0) //check ":" then rest of the name
		{
			// std::cout << "entered token[3]" << std::endl;
			for (unsigned long i = 3; i < tokens.size(); i++)
			{
				clients[client_fd].setRealname(tokens[i]);
			}
			// std::cout << "get client username: " << clients[client_fd].getUsername() << std::endl;
			return true;
		}
	}
	// std::cout << "returned false at the end" << std::endl;
	return false;
}