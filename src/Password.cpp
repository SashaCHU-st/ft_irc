/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Password.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 15:54:52 by alli              #+#    #+#             */
/*   Updated: 2025/01/08 11:52:11 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"

bool Serv::authenticate_password(int fd, std::vector<std::string> tokens)
{
	if (tokens.size() > 1)
	{
		// std::cerr << "please only put /pass "
		std::string error_msg = "Please only put: /pass <password>";
		send(fd, error_msg.c_str(), error_msg.size(), 0);
	}
	else {
		std::cout << "token: " << tokens[0] << std::endl;
		std::string token = tokens[0];
		std::string pass = get_pass();
		if (pass == token)
		{
			//have a flag to check client?
			return true;
		}
		else
		{
			std::string error_pass = "Wrong password, please restart\n";
			//send(client_fd, error_pass.c_str(), error_msg.size(), 0);
			return false;
		}
	}
	return false;
}