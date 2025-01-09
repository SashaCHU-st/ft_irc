/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 13:17:13 by alli              #+#    #+#             */
/*   Updated: 2025/01/08 16:06:01 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <vector>
#include <iostream>
#include <string>

class Client {
	private:
		std::string _username;
		std::string _nickname = "";
		std::string _password;
		int _fd;
	public:
		Client();
		~Client();
		std::string getUsername();
		std::string getNickname();
		std::string getPassword();
		int 		getFd();
		
		//setters
		void setFd(int fd);
		void setNickname(std::string nickname);
		void setUsername(std::string username);
		// std::vector<std::string> getServerInfo();
};

#endif
