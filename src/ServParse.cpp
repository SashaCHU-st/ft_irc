/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServParse.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 15:32:10 by alli              #+#    #+#             */
/*   Updated: 2025/01/08 11:46:25 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"

void Serv::parse_command(int fd, const std::string& line) {
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
		std::string pong = std::string("PONG") + "/r/n";
		//send(client_fd, pong.c_str(), pong.size(), 0);
	}
	if (tokens.empty())
	{
		std::cerr << "Please add another parameter" << std::endl;
		return ;
	}
	if (cmd == "CAP")
	{
		std::cout << "CAP" << std::endl;
	}
	if (cmd == "PASS")
	{
		std::cout << "password " << std::endl;
		if (authenticate_password(fd, tokens) == true)
		{
			Client client(fd);
			clients.push_back(client);
			return;
		}
		else
		{
			return;
		}
	}
	// if (cmd == "NICK")
	// {
	// 	//addNickname
	// }
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