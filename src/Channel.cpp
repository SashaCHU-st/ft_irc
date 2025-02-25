/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 16:13:44 by epolkhov          #+#    #+#             */
/*   Updated: 2025/02/21 14:36:25 by alli             ###   ########.fr       */
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


void Channel::addUser(Client* client) {
	if (std::find(_users.begin(), _users.end(), client) == _users.end()) {
		_users.push_back(client);
		if (_users.size() == 1) {
			addOperator(client);
		}
		// std::cout << "Added user " << client->getNickname() << " to channel " << _name << "." << std::endl;
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

Client*	Channel::getOperator(Client* client) const {
	if (!client)
		return nullptr;
	if (isOperator(client) == true){
		return client;
	}
	else 
		return nullptr;
}

std::string Channel::getUsersNick() const {
	std::string names;
	std::vector<Client*> users = getUsers();
	for(size_t i = 0; i < getUsers().size(); i++)
	{
		if (i == getUsers().size() - 1)
		{
			if (isOperator(users[i]) == true)
				names += "@" + users[i]->getNickname() + " ";
			names += users[i]->getNickname() + " ";
			return names;
		}
		if (isOperator(users[i]) == true)
		{
			names += "@" + users[i]->getNickname() + " ";
		}
		else
			names += users[i]->getNickname() + " ";
	}
	return names;
}

void Channel::sendToAll(const std::string& message)
{
	for (size_t i = 0; i < _users.size(); ++i)
	{
		int tmpFd = _users[i]->getFd();
		send(tmpFd, message.c_str(), message.size(), 0);
	}
}

void Channel::broadcastMessage(const std::string& sender, const std::string& command, const std::string& message) {
	for (size_t i = 0; i < _users.size(); ++i) {
        int user_fd = _users[i]->getFd();
		
		std::string formattedMessage;
		if (isOperator(_users[i]))
		{
			formattedMessage = ":@" + sender + " " + command + " " + _name + " :" + message + "\r\n";
		}
		else{
			// std::cout<< "Not operator"<< std::endl;
			formattedMessage = ":" + sender + " " + command + " " + _name + " :" + message + "\r\n";
		}
        if (send(user_fd, formattedMessage.c_str(), formattedMessage.size(), 0) == -1) {
            std::cerr << "Failed to send message to user: " << _users[i]->getNickname() << std::endl;
        } 
		else {
            // std::cout << "Message sent to " << _users[i]->getNickname() << ": [" << sender << "] " << message << std::endl;
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
					// std::cout<< "Client "<< client->getNickname()<< "is alredy operator."<< std::endl;
				}
            } else {
                if (isOperator(client))
				{
                	removeOperator(client);
				}
				else{
					// std::cout << "Client "<< client->getNickname()<< " is not an operator"<< std::endl;
				}
            }
            break;
	}
}

bool Channel::setTopic(const std::string& topic, Client* client) {
	if (client == nullptr)
	{
		_topic = topic;
		return true;
	}
	else if (!_topicRestricted)
    {
        _topic = topic;
		return true;
    }
	else if (isOperator(client)) {
		_topic = topic;
		return true;
	} else {
		std::cout << "You do not have permission to change the topic.\n";
		std::string message = ":" + client->getServerName() + " 481 "+ client->getNickname() +
						 ": You do not have permission to change the topic.\r\n";
		send(client->getFd(), message.c_str(), message.size(), 0);
		return false;
	}
	return true;
}

bool Channel::checkPassword(const std::string& password) const {
	return _password.empty() || _password == password;
}