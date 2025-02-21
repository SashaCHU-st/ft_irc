/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Password.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 15:54:52 by alli              #+#    #+#             */
/*   Updated: 2025/02/21 08:41:55 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"

bool Serv::authenticate_password(int fd, std::vector<std::string> tokens)
{
	if (tokens.size() > 1)
	{
		std::string error_msg = "Please only put: /pass <password>";
		send(fd, error_msg.c_str(), error_msg.size(), 0);
	}
	else {
		std::string token = tokens[0];
		std::string pass = get_pass();
		if (pass == token)
		{
			clients[fd].passwordCheck = true;
			return true;
		}
		else
		{
			sendError(fd, "ERR_PASSWDMISMATCH", 464);
			close(fd);
			return false;
		}
	}
	close(fd);
	return false;
}