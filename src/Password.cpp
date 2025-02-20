/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Password.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 15:54:52 by alli              #+#    #+#             */
/*   Updated: 2025/02/19 13:35:45 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"

bool Serv::authenticate_password(int fd, std::vector<std::string> tokens)
{
	std::cout << "Entered authenticate password" << std::endl;
	if (tokens.size() > 1)
	{
		// std::cerr << "please only put /pass "
		std::string error_msg = "Please only put: /pass <password>";
		send(fd, error_msg.c_str(), error_msg.size(), 0);
	}
	else {
		// std::cout << "token: " << tokens[0] << std::endl;
		std::string token = tokens[0];
		std::string pass = get_pass();
		if (pass == token)
		{
			clients[fd].passwordCheck = true;
			return true;
		}
		else
		{
			std::string error_pass = "Wrong password, please restart\n";
			std::cout << error_pass << std::endl;
			close(fd);
			return false;
		}
	}
	close(fd);
	return false;
}