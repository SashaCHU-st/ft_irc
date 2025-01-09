/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServParse.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 15:32:10 by alli              #+#    #+#             */
/*   Updated: 2025/01/09 13:41:40 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"

int Serv::parse_command(int fd, const std::string& line) {
	// std::cout << line << std::endl;
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
	if (tokens.empty())
	{
		std::cerr << "Please add another parameter" << std::endl;
		return 1;
	}
	if (cmd == "CAP")
	{
		std::string cap = tokens[0] + "\r\n";
		return 0;
	}
	if (cmd == "PASS")
	{
		// std::cout << "password " << std::endl;
		if (authenticate_password(fd, tokens) == true)
		{
			Client client;
			client.setFd(fd);
			client.setNickname("");
			client.setUsername("");
			clients.push_back(client); //create client later once password is correct
			return 0;
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
			std::string nick = "nickname " + tokens[0] + " added \r\n";
			send(fd, nick.c_str(), nick.size(), 0);
			return 0;
		}
		else
		{
			std::string ERR_NICKNAMEINUSE = std::string("No nickname or in use") + "\r\n";
			send(fd, ERR_NICKNAMEINUSE.c_str(), ERR_NICKNAMEINUSE.size(), 0);
			return 1;
		}
	}
	return 0;
	if (cmd == "USER")
	{
		if (addUser(fd, tokens) == true)
		{
			std::string user = "Username added" + tokens[0] + "\r\n";
			send(fd, user.c_str(), user.size(), 0);
		}
		else
		{
			std::string error_username = "No username added \r\n";
			send(fd, error_username.c_str(), error_username.size(), 0);
			return 1;
		}
	}
	if (cmd == "PING")
	{
		std::string pong = std::string("PONG") + "\r\n";
		send(fd, pong.c_str(), pong.size(), 0);
	}
	if (!clients[fd].getUsername().empty() && !clients[fd].getNickname().empty())
		clients[fd].allSet = true;
	
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