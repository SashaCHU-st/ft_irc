/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServParse.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 15:32:10 by alli              #+#    #+#             */
/*   Updated: 2025/01/08 16:14:54 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"

int Serv::parse_command(int fd, const std::string& line) {
	std::cout << line << std::endl;
	std::istringstream lss(line);
	std::vector<std::string> tokens;
	
	std::string token;
	std::string cmd;
	
	if (lss)
		lss >> cmd;
	while (lss >> token)
		tokens.push_back(token);
	if (cmd == "QUIT")
	{
		std::cout << "Thank you for using irSEE" << std::endl;
		exit(0); //close fds and exit function
	}
	else if (cmd == "PING")
	{
		std::string pong = std::string("PONG") + "\r\n";
		send(fd, pong.c_str(), pong.size(), 0);
	}
	if (tokens.empty())
	{
		std::cerr << "Please add another parameter" << std::endl;
		return 1;
	}
	if (cmd == "CAP")
	{
		std::cout << "CAP" << std::endl;
	}
	if (cmd == "PASS")
	{
		// std::cout << "password " << std::endl;
		if (authenticate_password(fd, tokens) == true)
		{
			Client client(fd);
			clients.push_back(client);
			return 0;
			
			// Client client; alice's
			// client.setFd(fd);
			// clients.push_back(client); //create client later once password is correct
			// return 0;
		}
		else
		{
			return 1;
		}
	}
	if (cmd == "NICK")
	{
		if (addNickname(fd, tokens) == true)
		{
			std::string nick = std::string("nickname added: ") + "\r\n";
			send(fd, nick.c_str(), nick.size(), 0);
			return 0;
		}
		else
			return 1;
	}
	return 0;
	// if (cmd == "USER")
	// {
	// 	//addUser
	// }
	// if (cmd == "JOIN")
	// {
		
	// }
	
	// if (cmd == "PRVMSG")
	// {
		
	// }
	// if (cmd == "TOPIC")
	// {
		
	// }
	// if (cmd == "MODE")
	// {
		
	// }
	// if (cmd == "KICK")
	// {
		
	// }
	// if (cmd == "INVITE")
	// {
		
	// }
	// if (cmd == "PART")
	// {
	// 	//leaving the channel
	// }
}