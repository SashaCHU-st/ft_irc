/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelCMDs.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: epolkhov <epolkhov@student.42.fr>          #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-01-10 12:59:50 by epolkhov          #+#    #+#             */
/*   Updated: 2025-01-10 12:59:50 by epolkhov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serv.hpp"

int checkChanName(std::string name)
{
	std::vector<char> chars = {'!', '@', '$', '%', ' '};
	for (char c : name)
	{
		if (std::find(chars.begin(), chars.end(), c) != chars.end() || \
				std::iscntrl(static_cast<unsigned char>(c))) {
            return 1;  // Invalid: contains forbidden character or control character
        }
	}
	return 0;
}

int Serv::cmdJOIN(int fd, std::vector<std::string> line)
{
	if (line.empty())
	{
		std::cout<<"No parameter after JOIN"<< std::endl;// ERR_NEEDMOREPARAMS (461)
		return 1;
	}
	if (line.size() > 2)
	{
		std::cout<< "Too many parameters for the JOIN command."<< std::endl;
		return 1;
	}
	std::string chanCheck = line[0];
	std::string keyCheck;
	if (line.size() > 1)
	{
		keyCheck = line[1];
	}
	else{
		keyCheck = "";
	}
	std::vector<std::string> channels;
	std::vector<std::string> keys;
	if (chanCheck.find(',') != std::string::npos)
	{
		channels = splitStr(chanCheck, ",");
	}
	else{
		channels.push_back(chanCheck);
	}
	if (keyCheck.find(',') != std::string::npos)
	{
		keys = splitStr(keyCheck, ",");
	}
	else{
		keys.push_back(keyCheck);
	}
	Client* client = getClientByFd(fd);
	if (!client) {
        std::cout << "Client not found for fd: " << fd << std::endl;
        return 1;
    }
	if (channels.size() != keys.size() && keys.size() != 1) {
        std::cout << "Number of keys doesn't match the number of channels." << std::endl;
        return 1;
    }
	for (size_t i = 0; i < channels.size(); i++)
	{
		const std::string& chan = channels[i];
		if (chan.empty() || chan[0] != '#' || checkChanName(chan) == 1)
		{
			std::cout<< "Invalid channel name: "<< chan<< ": it should start with #"<<std::endl;
			continue ;
		}
		auto findChan = _channels.find(chan);
		std::shared_ptr<Channel> newChannel;
		if (findChan == _channels.end())
		{
			newChannel = createChannel(chan);
			std::string defaultTopic = "No topic set";
			newChannel->setTopic(defaultTopic, nullptr);
			if (keys.size() > 1)
			{
				newChannel->setPassword(keys[i]);
				std::cout << "Password set for channel " << chan << ": " << keys[i] << std::endl;
			}
			if (keys.size() == 1)
			{
				newChannel->setPassword(keys[0]);
				std::cout << "Password set for channel " << chan << ": " << keys[0] << std::endl;
			}
		}
		else
		{
			std::cout<< "Channel: "<< chan <<" already exists." << std::endl;
			newChannel = findChan->second;
			std::cout<< "Invite only mode : " << newChannel->isInviteOnly()<< std::endl;
			if (!newChannel->getPassword().empty())
			{
				if (i < keys.size() && !keys[i].empty() && !newChannel->checkPassword(keys[i]))
				{
					std::cout << "Incorrect password for channel: " << chan << std::endl;
            		continue;
				}
			}
			if (newChannel->isUserInChannel(client)) {
				std::cout << "User " << client<< " is already in the channel: "<< newChannel->getName() << std::endl;
				continue ;
			}
			if (newChannel->getUserLimit() == newChannel->getUserCount())
			{
				std::cout<< "User "<< client->getNickname() << " cannot join the channel as the ammount of users are limited."<< std::endl;
				continue ;
			}
			if (newChannel->isInviteOnly())
			{
				std::cout<< "User "<< client->getNickname()<< " tries to join the channel and wait for INVITE." << std::endl;
				continue ;
			}
		}
		newChannel->addUser(client);
		client->joinChannel(newChannel);
		newChannel->broadcastMessage(client->getNickname(), "has joined the channel");
	}
	return  0;
}

int Serv::cmdPART(int fd, std::vector<std::string> line)
{
	if (line.empty())
	{
		std::cout<<"Not enough parameters for PART command"<< std::endl; // ERR_NEEDMOREARAMS(461)
		return 1;
	}
	if (line.size() > 2)
	{
		std::cout<< "Too many parameters for the PART command."<< std::endl;
		return 1;
	}
	std::string chanCheck = line[0];
	std::vector<std::string> channels;

	if (chanCheck.find(',') != std::string::npos)
	{
		channels = splitStr(chanCheck, ",");
		for (const std::string& channel : channels)
		{
			if (channel.empty() || channel[0] != '#' || checkChanName(channel) == 1)
        	{
            std::cout << "Invalid channel name: " << channel << std::endl;
            return 1;
        	}
		}
	}
	if (chanCheck[0] == '#' && checkChanName(chanCheck) != 1)
	{
		channels.push_back(chanCheck);
	}
	else
	{
		std::cout<< "No channels found: incorrect input for PART."<< std::endl;
		return 1;
	}
	std::string reason;
	if (line.size() > 1)
	{
		reason = line[1];
	}
	for (const std::string& channelName : channels)
	{
		auto it = _channels.find(channelName);
		if (it == _channels.end())
		{
			std::cout<< "Channel "<< channelName<< " doesn't exist."<< std::endl;//ERR_NOSUCHCHANNEL(403)
			//return 1;
			continue ;
		}
		std::shared_ptr<Channel> channel = it->second;

		Client* client = getClientByFd(fd);
		if (!client) {
            std::cout << "Client not found by fd: " << fd << std::endl;
            return 1;
        }

		if (channel->isUserInChannel(client))
		{
			if (channel->isOperator(client))
			{
				channel->removeUser(client);
				client->leaveChannel(channel);
				if (!channel->getUsers().empty())
            	{
               	 	std::vector<Client*> usersInChannel = channel->getUsers();
                	srand(time(0));
                	int randomIndex = rand() % usersInChannel.size();
               		Client* randomUser = usersInChannel[randomIndex];
                	channel->addOperator(randomUser);
                	std::cout << "Assigned user " << randomUser->getNickname() << " as operator in channel " << channelName << std::endl;
            	}
			}
			else{
				channel->removeUser(client);
				client->leaveChannel(channel);
			}
		}
		else
		{
			std::cout<< "User "<< client->getNickname() <<" is not in channel "<< channelName<< std::endl;//ERR_NOTONCHANNEL(442)
			//return 1;
			continue ;
		}
		std::string message = "left channel " + channelName;
		if (!reason.empty())
		{
			message += " (" + reason + ")";
			channel->broadcastMessage(client->getNickname(), message);
		}
		else{
			channel->broadcastMessage(client->getNickname(), message);
		}
	}
	return 0;
}

int Serv::cmdINVITE(int fd, std::vector<std::string> line)
{
	if (line.empty())
	{
		std::cout<<"Not enough parameters for INVITE command"<< std::endl; // ERR_NEEDMOREARAMS(461)
		return 1;
	}
	if (line.size() != 2)
	{
		std::cout<< "Too many parameters for the INVITE command."<< std::endl;
		return 1;
	}
	std::string newUser = line[0];
	std::string chanToAdd =line[1];
	if (chanToAdd[0] != '#' || chanToAdd.empty() || checkChanName(chanToAdd) == 1)
	{
		std::cout<< "Invalid channel name."<<std::endl;
		return 1;
	}
	Client* client = getClientByFd(fd);
	if (!client) {
        std::cout << "Client not found for fd: " << fd << std::endl;
        return 1;
    }
	auto chanToFind = _channels.find(chanToAdd);
	if (chanToFind == _channels.end())
	{
		std::cout<< "Channel "<< chanToAdd<< " doesn't exist."<< std::endl;//ERR_NOSUCHCHANNEL(403)
		return 1;	
	}
	std::shared_ptr<Channel> channel = chanToFind->second;
	if (!channel->isUserInChannel(client))
	{
		std::cout<< "Client "<< client->getNickname()<< " not on channel."<<std::endl;// ERR_NOTONCHANNEL(403)
		return 1;
	}
	if (!channel->isOperator(client))
	{
		std::cout<<"Client is not an operator."<<std::endl;// ERR_CHANOPRIVSNEEDED(482)
		return 1;
	}
	Client* invitee = getClientByNickname(newUser);
	if (!invitee)
	{
		std::cout<< "User "<<newUser<< " not found."<< std::endl;
		return 1;
	}
	if (channel->isUserInChannel(invitee)) {
        std::cout << "User " << newUser << " is already in channel " << chanToAdd << "." << std::endl;//ERR_USERONCHANNEL(443)
        return 1;
    }
	channel->addUser(invitee);
	invitee->joinChannel(channel);
	std::string message = client->getServerName() + "INVITE " + invitee->getNickname() + channel->getName() + " :" + invitee->getNickname();
	send_message(invitee->getFd(), message);
	std::cout << "User " << newUser << " successfully invited to channel " << chanToAdd << "." << std::endl;
	return 0;
}

int checkValidMode(char mode)
{
	std::vector<char> validModes = {'i', 't', 'k', 'o', 'l', '+', '-'};
	for (char c : validModes)
	{
		if (mode == c)
		{
			return 1;
		}
	}
	std::cout << "Invalid mode character: " << mode << std::endl;
	return 0;
}

int checkDigit(std::string& str)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		if (!std::isdigit(str[i]))
		{
			return 1;
		}
	}
	return 0;
}

int Serv::cmdMODE(int fd, std::vector<std::string> line)
{
	if (line.empty() || line.size() < 2 || line.size() > 3)
	{
		std::cout << "Invalid number of parameters for MODE command."<< std::endl;
		return 1;
	}
	std::string chan = line[0];
	std::string mode = line[1];
	std::string param;
	if (chan[0] != '#' && mode[0] == '+' && mode[1] == 'i')
	{
		return 0;
	}
	if (line. size() == 3)
	{
		param = line[2];
	}
	else{
		param = "";
	}
	if (chan[0] != '#' || checkChanName(chan) == 1)
	{
		std::cout<<"Invalid channel name."<< std::endl;
		return 1;
	}
	auto findChan = _channels.find(chan);
	if (findChan == _channels.end())
	{
		std::cout<< "Channel "<< chan<< " doesn't exist."<< std::endl;//ERR_NOSUCHCHANNEL(403)
		return 1;	
	}
	std::shared_ptr<Channel> channel = findChan->second;
	Client* client = getClientByFd(fd);
	if (!client) {
        std::cout << "Client not found for fd: " << fd << std::endl;
        return 1;
    }
	Client* clientToAdd = getClientByNickname(param);
	std::cout<< "Print the mode size "<< mode.size()<< std::endl;
	// if (mode[0] == '+' || mode[0] == '-')
	// {
		for (size_t i = 0; i < mode.size() ; i++)
		{
			if (mode[i] == '+' || mode[i] == '-')
			{
				i++;
			}
			std::cout << "entered mode function" << std::endl;
			if (checkValidMode(mode[i]) == 0) {
            	std::cout << "Invalid mode character." << std::endl;
            	return 1;
        	}
			if (mode[i] == 'l' && (param.empty() || checkDigit(param) == 1))
			{
				std::cout <<"Mode set 'l' should contain only digits as a parameter and always require parameter."<< std::endl;
			}
			if (mode[i] == 'k' && param.empty())
			{
				std::cout<< "Parameter cannot be empty for the mode set 'k'"<<std::endl;
				return 1;
			}
			if (mode[i] == 'o' && (param.empty() || !channel->isUserInChannel(clientToAdd)))
			{
				std::cout<< "Parameter cannot be empty for the mode set 'o' or user is not in channel."<<std::endl;
				return 1;
			}
			if (channel->isOperator(client))
			{
				if (mode[0] == '+' )
				{
					std::cout<< "Mode + before "<< channel->isInviteOnly() <<std::endl;
					channel->setMode(mode[i], true, param, clientToAdd);
					std::cout<< "Mode + After"<< channel->isInviteOnly() <<std::endl;
				}
				else if (mode[0] == '-')
				{
					std::cout<< "Mode + before "<< channel->isInviteOnly() <<std::endl;
					channel->setMode(mode[i], false, param, clientToAdd);
					std::cout<< "Mode + before "<< channel->isInviteOnly() <<std::endl;
				}
			}
			else{
				std::cout << "Client is not an operator." << std::endl;
    			return 1;
			}	
		}
	//}
	// else{
	// 	std::cout<< "Incorrect mode settings."<< std::endl;
	// 	return 1;
	// }
	return 0;
}

int Serv::cmdKICK(int fd, std::vector<std::string> line)
{
	std::cout << "Line size: "<< line.size()<< std::endl;
	if (line.empty() || line.size() < 3)
	{
		std::cout << "Invalid number of parameters for KICK command."<< std::endl;
		return 1;
	}
	std::string checkChan = line[0];
	if (checkChan[0] != '#' || checkChanName(checkChan) == 1)
	{
		std::cout<< "Invalid channel name."<<std::endl;
		return 1;
	}
	auto findChan = _channels.find(checkChan);
	if (findChan == _channels.end())
	{
		std::cout<< "Channel "<< checkChan<< " doesn't exist."<< std::endl;//ERR_NOSUCHCHANNEL(403)
		return 1;	
	}
	std::shared_ptr<Channel> channel = findChan->second;
	Client* client = getClientByFd(fd);
	std::vector<std::string> reason;
	if (!client) {
        std::cout << "Client not found for fd: " << fd << std::endl;
        return 1;
    }
	std::vector<std::string> usersToKick;
	std::string message;
	std::cout << "Line size: "<< line.size()<< std::endl;
	for (size_t i = 1; i < line.size(); ++i)
	{
		std::cout << "Print i: "<< i<< std::endl;
		Client* userKick = getClientByNickname(line[i]);
		std::cout << "Print usetToKick: "<< userKick<< std::endl;
		if (!userKick || !channel->isUserInChannel(userKick))
		{
			std::cout<< "User "<< line[i]<< " not found."<< std::endl;
			reason.push_back(line[i]);
			continue ;
		}	
		if (channel->isOperator(client))
		{
			channel->removeUser(userKick);
			userKick->leaveChannel(channel);
			usersToKick.push_back(userKick->getNickname());
			std::cout<< "The user "<< userKick->getNickname() << " is kicked out from the channel "<< channel->getName()<< std::endl;
		}
		else{
			std::cout << "You do not have permission to kick users from this channel." << std::endl;
            return 1;
		}	
	}

	std::string fullReason;
    if (!reason.empty())
    {
        std::cout << "I am here in reason" << std::endl;
		for (size_t i = 0; i < reason.size(); ++i)
        {
            fullReason += reason[i];
            if (i < reason.size() - 1) // Add space between words if it's not the last one
            {
                fullReason += " ";
            }
        }
    }
	std::cout << "Print full reason: "<< fullReason<<  std::endl;
	for (size_t i = 0; i < usersToKick.size(); i++)
	{
		std::cout << "Check to broadcast" << std::endl;
		message = "User " + usersToKick[i] + " is kicked out of the channel: " + channel->getName();
		if (!fullReason.empty())
		{
			message += " (" + fullReason + ")";
		}
		channel->broadcastMessage(client->getNickname(), message);
	}
	return 0;
}

int Serv::cmdTOPIC(int fd, std::vector<std::string> line)
{
	if (line.empty())
	{
		std::cout << "Invalid number of parameters for TOPIC command."<< std::endl;
		return 1;
	}
	std::string checkChan = line[0];
	if (checkChan[0] != '#' || checkChanName(checkChan) == 1)
	{
		std::cout<< "Invalid channel name."<<std::endl;
		return 1;
	}
	auto findChan = _channels.find(checkChan);
	if (findChan == _channels.end())
	{
		std::cout<< "Channel "<< checkChan<< " doesn't exist."<< std::endl;//ERR_NOSUCHCHANNEL(403)
		return 1;	
	}
	std::shared_ptr<Channel> channel = findChan->second;
	Client* client = getClientByFd(fd);
	std::vector<std::string> reason;
	if (!client) {
        std::cout << "Client not found for fd: " << fd << std::endl; // ERR_NOTONCHANNEL (442)
        return 1;
    }
	std::vector<std::string> topicArr;
	if (line.size() > 1)
	{
		for (size_t i = 1; i < line.size(); ++i)
		{
			topicArr.push_back(line[i]);
		}
	}else
	{
		std::cout<< "No topic is set for channel: "<< channel->getName()<< std::endl;
		return 0;
	}
	std::string topic;
	if (!topicArr.empty())
	{
		if (topicArr.size() == 1 && topicArr[0] == ":")
		{
			topic = "";
		}
		for (size_t i = 0; i < topicArr.size(); ++i)
		{
			topic += topicArr[i];
            if (i < topicArr.size() - 1) // Add space between words if it's not the last one
            {
                topic += " ";
            }
		}
		channel->setTopic(topic, client);
	}
	else{
		std::cout << "Current topic for channel " << channel->getName() << ": " << channel->getTopic() << std::endl;
		channel->broadcastMessage(client->getNickname(), " topic of a chennel " + channel->getName() + channel->getTopic());
	}
	return 0;
}
