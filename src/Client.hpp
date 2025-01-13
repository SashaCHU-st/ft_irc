/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 13:17:13 by alli              #+#    #+#             */
/*   Updated: 2025/01/13 13:04:09 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #ifndef CLIENT_HPP
// #define CLIENT_HPP

// #include <vector>
// #include <iostream>
// #include <string>

// class Client {
// 	private:
// 		int _fd;
// 		std::string _username;
// 		std::string _nickname;
// 		std::string _password;
// 		std::string _hostname;
// 		std::string _servername;
// 		std::vector<std::string> _realname;
// 	public:
// 		// Client(); 
// 		Client(int socket_fd, const std::string& _password); 
// 		~Client();

// 		std::string getUsername();
// 		std::string getNickname() const;
// 		std::string getPassword();
// 		int 		getFd () const;
// 		std::string getHostName();
// 		// int 		getFd();
// 		bool allSet = false;
// 		bool welcomeSent = false;
		
// 		//setters
// 		void setFd(int fd);
// 		void setNickname(std::string nickname);
// 		void setUsername(std::string username);
// 		void setHostName(std::string hostname);
// 		void setServername(std::string servername);
// 		void setRealname(std::string names);
		
// 		// std::vector<std::string> getServerInfo();
// };

// #endif
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <vector>
#include <iostream>
#include <string>

class Client {
	private:
		std::string _username;
		std::string _nickname; //unique
		std::string _password;
		std::string _hostname;
		std::string _servername;
		std::vector<std::string> _realname;
		int _fd;//unqiue
	public:
		Client(); 
		Client(int fd);
		~Client();

		std::string getUsername();
		std::string getNickname() const;
		std::string getPassword();
		int 		getFd () const;
		std::string getHostName();
		// int 		getFd();
		bool allSet = false;
		bool welcomeSent = false;
		bool passwordCheck = false;
		
		//setters
		void setFd(int fd);
		void setNickname(std::string nickname);
		void setUsername(std::string username);
		void setHostName(std::string hostname);
		void setServername(std::string servername);
		void setRealname(std::string names);
		
		// std::vector<std::string> getServerInfo();
};

#endif