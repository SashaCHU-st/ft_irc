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

// Constructor
Channel::Channel(const std::string& name)
	: _name(name), _userLimit(0), _exist(false), _inviteOnly(false), _topicRestricted(false) {}

// Destructor
Channel::~Channel() {}

// Getters
std::string Channel::getName() const {
	return _name;
}

std::string Channel::getTopic() const {
	return _topic;
}

int Channel::getUserCount() const {
	return _users.size();
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
			addOperator(client);  // First user is an operator
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

// Operator Management
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
void Channel::broadcastMessage(const std::string& sender, const std::string& message) {
	for (size_t i = 0; i < _users.size(); ++i) {
		std::cout << "Message to " << _users[i]->getNickname() << ": [" << sender << "] " << message << std::endl;
	}
}

// Set Modes (i, t, k, l)
void Channel::setMode(char mode, bool enable, const std::string& param) {
	switch (mode) {
		case 'i':
			_inviteOnly = enable;
			break;
		case 't':
			_topicRestricted = enable;
			break;
		case 'k':
			if(enable)
				_password = param;
			else
				_password = "";
			break;
		case 'l':
			if (enable)
				_userLimit = std::stoi(param);
			else
				_userLimit = 0;
			break;
		case 'r': // Reset all restrictions (default unrestricted state)
			_inviteOnly = false;
			_topicRestricted = false;
			_password = "";
			_userLimit = 0;
			std::cout << "All channel restrictions have been reset to default." << std::endl;
			break;
		default:
			std::cout << "Unknown mode: " << mode << std::endl;
			break;

	}
}

// Set Channel Topic
void Channel::setTopic(const std::string& topic, Client* client) {
	if (client == nullptr)
	{
		_topic = topic;
	}
	else if (!_topicRestricted || isOperator(client)) {
		_topic = topic;
		broadcastMessage(client->getNickname(), "Topic changed to: " + topic);
	} else {
		std::cout << "You do not have permission to change the topic.\n";
	}
}

// Check Channel Password
bool Channel::checkPassword(const std::string& password) const {
	return _password.empty() || _password == password;
}