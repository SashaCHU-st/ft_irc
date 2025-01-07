/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServParse.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 15:32:10 by alli              #+#    #+#             */
/*   Updated: 2025/01/07 16:10:12 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"

void parse_command(int fd, std::string line) {
	std::cout << line << std::endl;
	if (line == "CAP")
	{
		std::cout << "CAP" << std::endl;
	}
	if (line == "PASS")
	{
		std::cout << "password " << std::endl;
		//checkPassword(line);
	}
	if (line == "NICK")
	{
		//addNickname
	}
	if (line == "USER")
	{
		//addUser
	}
	if (line == "JOIN")
	{
		
	}
	if (line == "PING")
	{
		
	}
	if (line == "PRVMSG")
	{
		
	}
	if (line == "QUIT")
	{
		
	}
	if (line == "TOPIC")
	{
		
	}
	if (line == "MODE")
	{
		
	}
	if (line == "KICK")
	{
		
	}
	if (line == "INVITE")
	{
		
	}
	if (line == "PART")
	{
		//leaving the channel
	}
}