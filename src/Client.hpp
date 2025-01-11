/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 13:17:13 by alli              #+#    #+#             */
/*   Updated: 2025/01/09 13:40:55 by alli             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include "Channel.hpp"

class Client {
	private:
		std::string _username;
		std::string _nickname;
		std::string _password;
		std::string _hostname;
		std::string _servername;
		std::vector<std::string> _realname;
		
		int _fd;

		std::vector<std::shared_ptr<Channel>> _joinedChannels;
		
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
		
		//setters
		void setFd(int fd);
		void setNickname(std::string nickname);
		void setUsername(std::string username);
		void setHostName(std::string hostname);
		void setServername(std::string servername);
		void setRealname(std::string names);
		
		// std::vector<std::string> getServerInfo();

		//Channel Handler for Client
		void joinChannel(std::shared_ptr<Channel> channel);
		void leaveChannel(std::shared_ptr<Channel> channel);
		//bool isInChannel(Channel *channel)const;
		const std::vector<std::shared_ptr<Channel>>& getJoinedChannels()const;
};

#endif
