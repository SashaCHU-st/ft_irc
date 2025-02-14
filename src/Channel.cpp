/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: epolkhov <epolkhov@student.42.fr>          #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-01-08 16:13:44 by epolkhov          #+#    #+#             */
/*   Updated: 2025-01-08 16:13:44 by epolkhov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Client.hpp"
#include "Serv.hpp"

Channel::Channel(const std::string& name)
	: _name(name), _password(""),_userLimit(0), _exist(false), _inviteOnly(false), _topicRestricted(false) {}


Channel::~Channel() {}


std::string Channel::getName() const {
	return _name;
}

std::string Channel::getTopic() const {
	return _topic;
}

int Channel::getUserCount() const {
	return _users.size();
}

int Channel::getUserLimit() const{
	return _userLimit;
}

 std::vector<Client*> Channel::getUsers() const {
        return _users;
    }

std::string Channel::getPassword() const {
    return _password;
}
bool Channel::doesExist() const {
	return _exist;
}

bool Channel::isInviteOnly() const {
	return _inviteOnly;
}

// User Management
void Channel::addUser(Client* client) {
	if (std::find(_users.begin(), _users.end(), client) == _users.end()) {
		_users.push_back(client);
		if (_users.size() == 1) {
			addOperator(client);
		}
		std::cout << "Added user " << client->getNickname() << " to channel " << _name << "." << std::endl;
    } else {
		 std::cout << "User " << client->getNickname() << " is already in channel " << _name << "." << std::endl;
	}
}

void Channel::removeUser(Client* client) {
	_users.erase(std::remove(_users.begin(), _users.end(), client), _users.end());
	_operators.erase(std::remove(_operators.begin(), _operators.end(), client), _operators.end());
}

bool Channel::isUserInChannel(Client* client) const {
	return std::find(_users.begin(), _users.end(), client) != _users.end();
}

void Channel::addOperator(Client* client) {
	if (std::find(_operators.begin(), _operators.end(), client) == _operators.end()) {
		_operators.push_back(client);
	}
}

void Channel::removeOperator(Client* client) {
	_operators.erase(std::remove(_operators.begin(), _operators.end(), client), _operators.end());
}

bool Channel::isOperator(Client* client) const {
	return std::find(_operators.begin(), _operators.end(), client) != _operators.end();
}

// Broadcast Messages
// void Channel::broadcastMessage(const std::string& sender, const std::string& message) {
// 	// for (size_t i = 0; i < _users.size(); ++i) {
// 	// 	std::cout << "Message to " << _users[i]->getNickname() << ": [" << sender << "] " << message << std::endl;
// 	// }
// 	std::cout << "Broadcasting message to " << _users.size() << " users." << std::endl;
// 	for (size_t i = 0; i < _users.size(); ++i) {
//     	std::cout << "User: " << _users[i]->getNickname() << " fd: " << _users[i]->getFd()<< std::endl;
// 	}
// 	for (size_t i = 0; i < _users.size(); ++i) {
//         // Get the file descriptor of the user
//         int user_fd = _users[i]->getFd();
// 		std::cout<<"Print out user_fd "<< user_fd<< std::endl;
//         // Format the message for the user
//         //std::string formattedMessage = "[" + sender + "] " + message + "\r\n";
// 		std::string formattedMessage = ":" + sender + " PRIVMSG " + _name + " :" + message + "\r\n";
// 		std::cout << "Sending message to fd " << user_fd << ": " << formattedMessage;
//         // Send the message to the user's file descriptor
//         if (send(user_fd, formattedMessage.c_str(), formattedMessage.size(), 0) == -1) {
//             std::cerr << "Failed to send message to user: " << _users[i]->getNickname() << std::endl;
//         } else {
//             std::cout << "Message sent to " << _users[i]->getNickname() << ": [" << sender << "] " << message << std::endl;
//         }
//     }
// }

void Channel::broadcastMessage(const std::string& sender, const std::string& command, const std::string& message) {
	// for (size_t i = 0; i < _users.size(); ++i) {
	// 	std::cout << "Message to " << _users[i]->getNickname() << ": [" << sender << "] " << message << std::endl;
	// }
	std::cout << "Broadcasting message to " << _users.size() << " users." << std::endl;
	for (size_t i = 0; i < _users.size(); ++i) {
    	std::cout << "User: " << _users[i]->getNickname() << " fd: " << _users[i]->getFd()<< std::endl;
	}
	for (size_t i = 0; i < _users.size(); ++i) {
        // Get the file descriptor of the user
        int user_fd = _users[i]->getFd();
		std::cout<<"Print out user_fd "<< user_fd<< std::endl;
        // Format the message for the user
        //std::string formattedMessage = "[" + sender + "] " + message + "\r\n";
		std::string formattedMessage = ":" + sender + " " + command + " " + _name + " :" + message + "\r\n";
		std::cout << "Sending message to fd " << user_fd << ": " << formattedMessage;
        // Send the message to the user's file descriptor
        if (send(user_fd, formattedMessage.c_str(), formattedMessage.size(), 0) == -1) {
            std::cerr << "Failed to send message to user: " << _users[i]->getNickname() << std::endl;
        } else {
            std::cout << "Message sent to " << _users[i]->getNickname() << ": [" << sender << "] " << message << std::endl;
        }
    }
}
void Channel::setPassword(const std::string& password)
{
	_password = password;
}

// Set Modes (i, t, k, l)
void Channel::setMode(char mode, bool enable, const std::string& param, Client* client)
{
	switch (mode) {
		case 'i':
			_inviteOnly = enable;
			break;
		case 't':
			_topicRestricted = enable;
			break;
		case 'k':

			if(enable ==  true)
				_password = param;
			else
				_password = "";
			break;
		case 'l':
			if (enable == true)
				_userLimit = std::stoi(param);
			else
				_userLimit = 0;
			break;
		case 'o':
            if (enable == true) {
                if (!isOperator(client))
				{
                	addOperator(client);
				}
				else{
					std::cout<< "Client "<< client->getNickname()<< "is alredy operator."<< std::endl;
				}
            } else {
                if (isOperator(client))
				{
                	removeOperator(client);
				}
				else{
					std::cout << "Client "<< client->getNickname()<< " is not an operator"<< std::endl;
				}
            }
            break;
	}
}

// Set Channel Topic
void Channel::setTopic(const std::string& topic, Client* client) {
	if (client == nullptr)
	{
		_topic = topic;
	}
	else if (!_topicRestricted)  // If the topic is not restricted, allow any user to set the topic
    {
        _topic = topic;
		//std::cout<< "Topic changed to "<< _topic<< std::endl;
        //broadcastMessage(client->getNickname(), "Topic changed to: " + topic);
    }
	else if (isOperator(client)) {
		_topic = topic;
		//broadcastMessage(client->getNickname(), "Topic changed to: " + topic);
	} else {
		std::cout << "You do not have permission to change the topic.\n";
	}
}

// Check Channel Password
bool Channel::checkPassword(const std::string& password) const {
	return _password.empty() || _password == password;
}