/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServPassword.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 15:54:52 by alli              #+#    #+#             */
/*   Updated: 2025/01/08 10:56:39 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"

bool Serv::authenticate_password(int fd, std::vector<std::string> tokens)
{
	(void) fd;
	if (tokens.size() > 1)
	{
		// std::cerr << "please only put /pass "
		std::string error_msg = "Please only put: /pass <password>";
		//send(client_fd, error_msg.c_str, error_msg.size(), 0);
	}
	else {
		std::cout << "token: " << tokens[0] << std::endl;
		std::string token = tokens[0];
		//check if token = server password
	}
	return(true);
}