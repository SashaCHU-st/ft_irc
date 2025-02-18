/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelCMDs.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 12:59:50 by epolkhov          #+#    #+#             */
/*   Updated: 2025/02/18 13:48:59 by alli             ###   ########.fr       */
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
		std::cout<<"No parameter after JOIN"<< std::endl;
		sendError(fd, "ERR_NEEDMOREPARAMS" , 461);
		return 1;
	}
	if (line.size() > 2)
	{
		std::cout<< "Too many parameters for the JOIN command."<< std::endl;
		sendError(fd, "ERR_TOOMANYPARAMS" , 461);
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
	for (size_t i = 0; keys.size() > i; i++)
	{
		std::cout << "Keys array: "<< keys[i]<< std::endl;
	}
	Client* client = getClientByFd(fd);
	if (!client) {
        std::cerr << "Client not found for fd: " << fd << std::endl;
		sendError(fd, "ERR_NOSUCHCLIENT", 4);
        return 1;
    }
	if (channels.size() != keys.size() && keys.size() != 1) {
        std::cerr << "Number of keys doesn't match the number of channels." << std::endl;
		sendError(fd, "ERR_BADCHANNELKEY: Number of keys doesn't match the number of channels",  475);
        return 1;
    }
	int count_joined = 0;
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (count_joined > 5)
		{
			sendError(fd, "ERR_TOOMANYCHANNELS", 405);
			return 1;
		}
		const std::string& chan = channels[i];
		if (chan.empty() || chan[0] != '#' || checkChanName(chan) == 1)
		{
			std::cout<< "Invalid channel name: "<< chan<< ": it should start with #"<<std::endl;
			sendError(fd, "ERR_NOSUCHCHANNEL", 403);
			continue ;
		}
		auto findChan = _channels.find(chan);
		std::shared_ptr<Channel> newChannel;
		if (findChan == _channels.end())
		{
			newChannel = createChannel(chan);
			std::string defaultTopic = "";
			newChannel->setTopic(defaultTopic, nullptr);
			newChannel->addUser(client);
			client->joinChannel(newChannel);
			if (newChannel->isOperator(client) == false)
			{
				std::string msgJoin = ":" + client->getNickname() + "!" + client->getUsername() + "@" 
					+ client->getServerName()
					+ " JOIN " + newChannel->getName() + "\r\n"; 
				std::string msg353 = ":" + client->getServerName() + " 353 " + client->getNickname() 
					+ " = " + newChannel->getName() + " :"
					+ newChannel->getUsersNick() + "\r\n";
				std::cout << "msg353 1:" << msg353 << std::endl;
				newChannel->sendToAll(msgJoin);
				newChannel->sendToAll(msg353);
			}
			else if (newChannel->isOperator(client) == true)
			{
				std::string modeMessage = ":" + client->getServerName() +
				" MODE " + newChannel->getName() +
				" +o " + client->getNickname() + "\r\n";
				std::string userList = "@" + newChannel->getOperator(client)->getNickname() + ""
					+ " " + newChannel->getUsersNick();
				std::string msgJoin = ":" + client->getNickname() + "!" + client->getUsername() + "@" 
					+ client->getServerName()
					+ " JOIN " + newChannel->getName() + "\r\n"; 
				std::string msg353 =  ":" + client->getServerName() + " 353 " + client->getNickname()
					+ " = " + newChannel->getName() + " :" + userList + "\r\n";
				std::cout << "msg353 2 " << msg353 << std::endl;
				std::cout << userList << std::endl;
				newChannel->sendToAll(modeMessage);
				newChannel->sendToAll(msgJoin);
				newChannel->sendToAll(msg353);
			}
			std::string msg366 = ":" + client->getServerName() + " 366 " + client->getNickname() 
				+ " " + newChannel->getName() + "\r\n";
			newChannel->sendToAll(msg366);
			count_joined++;
		}
		else
		{
			std::cout<< "Channel: "<< chan <<" already exists." << std::endl;
			newChannel = findChan->second;
			std::cout<< "Invite only mode : " << newChannel->isInviteOnly()<< std::endl;
			std::cout<< "Pasword : " << newChannel->getPassword()<< std::endl;
			if (!newChannel->getPassword().empty())
			{
				if (keys[i].empty())
				{
					std::cout<< "I am at keys"<< std::endl;
					sendError(fd, "ERR_BADCHANNELKEY" + client->getNickname() + " " + newChannel->getName() + " :Cannot join channel (+k) ", 475);
					// std::string msg = ": 475 " + client->getNickname() + " " + newChannel->getName() + " :Cannot join channel (+k)\r\n";
					// send(fd, msg.c_str(), msg.length(), 0);
					continue ;
				}
				if (!keys[i].empty() && !newChannel->checkPassword(keys[i]))
				{
					std::cout<< "Key 1: "<< keys[i]<< std::endl;
					std::cerr << "Incorrect password for channel: " << chan << std::endl;
					sendError(fd, "ERR_BADCHANNELKEY" + client->getNickname() + " " + newChannel->getName() + " :Incorrect password (+k) ", 475);
					// std::string msg = ":ft_irc 475 " + client->getNickname() + " " + newChannel->getName() + " :Incorrect password (+k)\r\n";
					// send(fd, msg.c_str(), msg.length(), 0);
					continue ;
				}
			}
			if (newChannel->isUserInChannel(client)) {
				std::cout << "User " << client<< " is already in the channel: "<< newChannel->getName() << std::endl;
				sendError(fd, "ERR_USER_ALREADY_IN_CHANNEL " + client->getNickname() + " " + newChannel->getName() + " :You are already in the channel", 433);
				continue ;
			}
			if (newChannel->getUserLimit() == newChannel->getUserCount())
			{
				std::cout<< "User "<< client->getNickname() << " cannot join the channel as the ammount of users are limited."<< std::endl;
				sendError(fd, "ERR_CHANNELISFULL" + client->getNickname() + " " + newChannel->getName() + " :Cannot join channel (+l)", 471);
				continue ;
			}
			if (newChannel->isInviteOnly())
			{
				std::cout<< "User "<< client->getNickname()<< " tries to join the channel and wait for INVITE." << std::endl;
				sendError(fd, "ERR_INVITEONLYCHAN" + client->getNickname() + " " + newChannel->getName() + " :Cannot join channel (+i)", 473);
				continue ;
			}		
			newChannel->addUser(client);
			client->joinChannel(newChannel);
			if (newChannel->isOperator(client) == false)
			{
				std::string msg353 = ":" + client->getServerName() + " 353 " + client->getNickname() 
				+ " = " + newChannel->getName() + " :"
				+ newChannel->getUsersNick() + "\r\n";
				std::cout << "msg353 3:" << msg353 << std::endl;
				std::string msgJoin = ":" + client->getNickname() + "!" + client->getUsername() + "@" 
					+ client->getServerName()
					+ " JOIN " + newChannel->getName() + "\r\n"; 
				std::cout << "msgJoin2: " << msgJoin << std::endl;
				newChannel->sendToAll(msgJoin);
				newChannel->sendToAll(msg353);
			}
			else if (newChannel->isOperator(client) == true)
			{
				std::string msg353 = ":" + client->getServerName() + " 353 " + client->getNickname()
					+ newChannel->getName() + " @" + newChannel->getOperator(client)->getNickname()
					+ newChannel->getUsersNick() + "\r\n";
				std::cout << "msg353 4:" << msg353 << std::endl;
				std::string msgJoin = ":" + client->getNickname() + "!" + client->getUsername() + "@" 
					+ client->getServerName() + " JOIN " + newChannel->getName() + "\r\n"; 
				std::cout << "msgJoin3: " << msgJoin << std::endl;
				newChannel->sendToAll(msgJoin);
				newChannel->sendToAll(msg353);
			}
			std::string msg366 = ":" + client->getServerName() + " 366 " + client->getNickname() 
				+ " " + newChannel->getName() + "\r\n";
			newChannel->sendToAll(msg366);
			std::string topic = newChannel->getTopic();
			if (!topic.empty())
			{
				// std::string topicMessage = "TOPIC " + newChannel->getName() + " :" + topic + "\r\n";
				// newChannel->broadcastMessage(client->getNickname(), "TOPIC", topicMessage);
				std::string topicMessage = ":ircserver 332 " + client->getNickname() + " " + newChannel->getName() + " :Welcome to " + topic + "\r\n";
                std::cout << topicMessage << std::endl;
				send(fd, topicMessage.c_str(), topicMessage.size(), 0);
			}
			count_joined++;
			
		}
        // Send the topic to the client
        // ssize_t bytesSent = send(fd, topicMessage.c_str(), topicMessage.size(), 0);
        // if (bytesSent == -1)
        // {
        //     std::cerr << "Error sending topic message to client " << fd << std::endl;
        // }
        // else
        // {
        //     std::cout << "Sent topic message to client " << fd << ": " << topic << std::endl;
        // }
	}
	return  0;
}

int Serv::cmdPART(int fd, std::vector<std::string> line)
{
	if (line.empty())
	{
		std::cerr << "Need more params" << std::endl;
		sendError(fd, "ERR_NEEDMOREARAMS: Need more params",  461);
		return 1;
		// std::cout<<"Not enough parameters for PART command"<< std::endl; // ERR_NEEDMOREARAMS(461)
		// return 1;
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
		std::cerr << "No such channel" << std::endl;
		sendError(fd, "ERR_NOSUCHCHANNEL  : No such channel",  403);
		return 1;
	}
	std::string reason;
	if (line.size() > 1)
	{
		reason = "";
    	for (size_t i = 1; i < line.size(); ++i) {
        	reason += line[i];
        	if (i != line.size() - 1) {
           	 reason += " ";
        	}
    	}
	}
	for (const std::string& channelName : channels)
	{
		auto it = _channels.find(channelName);
		if (it == _channels.end())
		{
			// std::cout<< "Channel "<< channelName<< " doesn't exist."<< std::endl;//ERR_NOSUCHCHANNEL(403)
			// //return 1;
			// continue ;
			std::cerr << "No such channel" << std::endl;
			sendError(fd, "ERR_NOSUCHCHANNEL  : No such channel",  403);
			continue;
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
				client->leaveChannel(channel->getName());
				if (!channel->getUsers().empty())
            	{
               	 	std::vector<Client*> usersInChannel = channel->getUsers();
                	srand(time(0)); // Seed the random number generator
                	int randomIndex = rand() % usersInChannel.size(); // Random index in range
               		Client* randomUser = usersInChannel[randomIndex];
                	channel->addOperator(randomUser);
                	std::cout << "Assigned user " << randomUser->getNickname() << " as operator in channel " << channelName << std::endl;
            	}
			}
			else{
				channel->removeUser(client);
				client->leaveChannel(channel->getName());
			}
		}
		else
		{
// <<<<<<< channel
// 			std::cout<< "User "<< client->getNickname() <<" is not in channel "<< channelName<< std::endl;//ERR_NOTONCHANNEL(442)
// 			continue ;
// =======
			// std::cout<< "User "<< client->getNickname() <<" is not in channel "<< channelName<< std::endl;//ERR_NOTONCHANNEL(442)
			// //return 1;
			// continue ;
			std::cerr << "No channel" << std::endl;
			sendError(fd, "ERR_NOTONCHANNEL: No channel",  442);
        	continue;
		}
		std::string message = channelName;
		if (!reason.empty()) {
    		message = ":" + client->getNickname() + " PART " + channelName + " :" + reason;
			send_message(client->getFd(), message);
		} else {
    		message = ":" + client->getNickname() + " PART " + channelName;
			send_message(client->getFd(), message);
		}
	}
	return 0;
}

int Serv::cmdINVITE(int fd, std::vector<std::string> line)
{
	if (line.empty())
	{
		std::cerr << "Need more params" << std::endl;
		sendError(fd, "ERR_NEEDMOREPARAMS : need more params",  461);
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
		std::cerr << "Nno suuch channel" << std::endl;
		sendError(fd, "ERR_NOSUCHCHANNEL : No such  channel",  403);
		return 1;
		// std::cout<< "Channel "<< chanToAdd<< " doesn't exist."<< std::endl;//ERR_NOSUCHCHANNEL(403)
		// return 1;	
	}
	std::shared_ptr<Channel> channel = chanToFind->second;
	if (!channel->isUserInChannel(client))
	{
		std::cerr << "Nno suuch channel" << std::endl;
		sendError(fd, "ERR_NOSUCHCHANNEL : No such  channel",  403);
		return 1;
		// std::cout<< "Client "<< client->getNickname()<< " not on channel."<<std::endl;// ERR_NOTONCHANNEL(403)
		// return 1;
	}
	if (!channel->isOperator(client))
	{
		std::cerr << "Client is not an operator." << std::endl;
		sendError(fd, "ERR_CHANOPRIVSNEEDED : Client is not an operator.",  482);
		return 1;
		// std::cout<<"Client is not an operator."<<std::endl;// ERR_CHANOPRIVSNEEDED(482)
		// return 1;
	}
	Client* invitee = getClientByNickname(newUser);
	if (!invitee)
	{
		std::cout<< "User "<<newUser<< " not found."<< std::endl;
		return 1;
	}
	if (channel->isUserInChannel(invitee)) {
		std::cerr << "channel already in use." << std::endl;
		sendError(fd, "ERR_USERONCHANNEL : channel already in use",  443);
		return 1;
        // std::cout << "User " << newUser << " is already in channel " << chanToAdd << "." << std::endl;//ERR_USERONCHANNEL(443)
        // return 1;
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
		std::cerr << "Nno suuch channel" << std::endl;
		sendError(fd, "ERR_NOSUCHCHANNEL : No such  channel",  403);
		return 1;
		// std::cout<< "Channel "<< chan<< " doesn't exist."<< std::endl;//ERR_NOSUCHCHANNEL(403)
		// return 1;	
	}
	std::shared_ptr<Channel> channel = findChan->second;
	Client* client = getClientByFd(fd);
	if (!client) {
        std::cout << "Client not found for fd: " << fd << std::endl;
        return 1;
    }
	Client* clientToAdd = getClientByNickname(param);
	std::cout<< "Print the mode size "<< mode.size()<< std::endl;
		for (size_t i = 0; i < mode.size() ; i++)
		{
			if (mode[i] == '+' || mode[i] == '-')
			{
				i++;
			}
			std::cout << "entered mode function" << std::endl;
			if (checkValidMode(mode[i]) == 0) {
            	std::cerr << "Invalid mode character." << std::endl;
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
					//std::cout<< "Mode + before "<< channel->isInviteOnly() <<std::endl;
					channel->setMode(mode[i], true, param, clientToAdd);
					//std::cout<< "Mode + After"<< channel->isInviteOnly() <<std::endl;
					std::string message = "MODE " + channel->getName() + " " + mode[i];
					channel->broadcastMessage(client->getNickname(), "MODE", message);
				}
				else if (mode[0] == '-')
				{
					//std::cout<< "Mode + before "<< channel->isInviteOnly() <<std::endl;
					channel->setMode(mode[i], false, param, clientToAdd);
					std::string message = "MODE " + channel->getName() + " " + mode[i];
					channel->broadcastMessage(client->getNickname(), "MODE", message);
					//std::cout<< "Mode + before "<< channel->isInviteOnly() <<std::endl;
				}
			}
			else{
				std::cerr << "Client is not an operator." << std::endl;
    			return 1;
			}	
		}
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
		std::cerr << "Nno suuch channel" << std::endl;
		sendError(fd, "ERR_NOSUCHCHANNEL : No such  channel",  403);
		return 1;
		// std::cout<< "Channel "<< checkChan<< " doesn't exist."<< std::endl;//ERR_NOSUCHCHANNEL(403)
		// return 1;	
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
			userKick->leaveChannel(channel->getName());
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
		channel->broadcastMessage(client->getNickname(), "KICK", message);
	}
	return 0;
}

// int Serv::cmdTOPIC(int fd, std::vector<std::string> line)
// {
// 	if (line.empty())
// 	{
// 		std::cout << "Invalid number of parameters for TOPIC command."<< std::endl;
// 		return 1;
// 	}
// 	std::string checkChan = line[0];
// 	if (checkChan[0] != '#' || checkChanName(checkChan) == 1)
// 	{
// 		std::cout<< "Invalid channel name."<<std::endl;
// 		return 1;
// 	}
// 	auto findChan = _channels.find(checkChan);
// 	if (findChan == _channels.end())
// 	{
// 		std::cout<< "Channel "<< checkChan<< " doesn't exist."<< std::endl;//ERR_NOSUCHCHANNEL(403)
// 		return 1;	
// 	}
// 	std::shared_ptr<Channel> channel = findChan->second;
// 	Client* client = getClientByFd(fd);
// 	std::vector<std::string> reason;
// 	if (!client) {
//         std::cout << "Client not found for fd: " << fd << std::endl; // ERR_NOTONCHANNEL (442)
//         return 1;
//     }
// 	std::vector<std::string> topicArr;
// 	if (line.size() > 1)
// 	{
// 		for (size_t i = 1; i < line.size(); ++i)
// 		{
// 			topicArr.push_back(line[i]);
// 		}
// 	}else
// 	{
// 		std::cout<< "No topic is set for channel: "<< channel->getName()<< std::endl;
// 		return 0;
// 	}
// 	std::string topic;
// 	if (!topicArr.empty())
// 	{
// 		if (topicArr.size() == 1 && topicArr[0] == ":")
// 		{
// 			topic = "";
// 		}
// 		for (size_t i = 0; i < topicArr.size(); ++i)
// 		{
// 			topic += topicArr[i];
//             if (i < topicArr.size() - 1) // Add space between words if it's not the last one
//             {
//                 topic += " ";
//             }
// 		}
// 		channel->setTopic(topic, client);
// 	}
// 	else{
// 		std::cout << "Current topic for channel " << channel->getName() << ": " << channel->getTopic() << std::endl;
// 		channel->broadcastMessage(client->getNickname(), " topic of a chennel " + channel->getName() + channel->getTopic());
// 	}
// 	return 0;
// }

int Serv::cmdTOPIC(int fd, std::vector<std::string> line)
{
	if (line.empty())
	{
		std::cerr << "Invalid number of parameters for TOPIC command."<< std::endl;
		return 1;
	}
	std::string checkChan = line[0];
	if (checkChan[0] != '#' || checkChanName(checkChan) == 1)
	{
		std::cerr<< "Invalid channel name."<<std::endl;
		return 1;
	}
	auto findChan = _channels.find(checkChan);
	if (findChan == _channels.end())
	{
		// std::cerr<< "Channel "<< checkChan<< " doesn't exist."<< std::endl;
		// sendError(fd, "Channel " + checkChan + " doesn't exist.", 403);//ERR_NOSUCHCHANNEL(403)
		// return 1;	
		std::cerr << "Nno suuch channel" << std::endl;
		sendError(fd, "ERR_NOSUCHCHANNEL : No such  channel",  403);
		return 1;
	}
	std::shared_ptr<Channel> channel = findChan->second;
	Client* client = getClientByFd(fd);
	std::vector<std::string> reason;
	if (!client) {
		std::cerr << "Client not found." << std::endl;
		sendError(fd, "ERR_NOTONCHANNEL : Client not found.",  442);
		return 1;
        // std::cerr << "Client not found for fd: " << fd << std::endl; // ERR_NOTONCHANNEL (442)
		// sendError(fd, "Client not found.", 442);
        // return 1;
    }
	if (!channel->isUserInChannel(client)) {
        // std::cerr << "User is not in channel " << channel->getName() << std::endl;
		// sendError(fd, "User is not in channel " + channel->getName(), 442); // ERR_NOTONCHANNEL (442)
        // return 1;
		std::cerr << "Client not found." << std::endl;
		sendError(fd, "ERR_NOTONCHANNEL : Client not found.",  442);
		return 1;
    }
	if (line.size() > 1) {
        std::string topic = "";
        for (size_t i = 1; i < line.size(); ++i) {
            topic += line[i];
            if (i < line.size() - 1) {
                topic += " ";
            }
        }
        if (topic[0] == ':') {
            topic = topic.substr(1);
        }
        channel->setTopic(topic, client);
		// for (size_t i = 0; i < channel->getUsers().size(); ++i){
		// 	std::string topicMessage = ":" + client->getNickname() + " TOPIC " + channel->getName() + " :" + topic + "\r\n";
		// 	ssize_t bytesSent = send(fd, topicMessage.c_str(), topicMessage.size(), 0);
		// 	if (bytesSent == -1) {
		// 		std::cerr << "Error sending TOPIC response to client " << fd << std::endl;
		// 	}

		// }
		std::string broadcastMessage = ":" + client->getNickname() + " TOPIC " + channel->getName() + " :" + topic + "\r\n";
        channel->broadcastMessage(client->getNickname(), "TOPIC", broadcastMessage);
	}
	else{
		std::string currentTopic = channel->getTopic();
        std::string topicResponse;

        if (currentTopic.empty()) {
            topicResponse = ":" " 331 " + client->getNickname() + " " + channel->getName() + " :No topic is set\r\n";
        }
        else {
            topicResponse = ":" " 332 " + client->getNickname() + " " + channel->getName() + " :" + currentTopic + "\r\n";
        }

        ssize_t bytesSent = send(fd, topicResponse.c_str(), topicResponse.size(), 0);
        if (bytesSent == -1) {
            std::cerr << "Error sending TOPIC response to client " << fd << std::endl;
        }
		//channel->broadcastMessage(client->getNickname(), "TOPIC", " topic of a chennel " + channel->getName() + channel->getTopic());
	}
	return 0;
}