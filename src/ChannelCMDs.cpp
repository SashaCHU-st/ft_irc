/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelCMDs.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 12:59:50 by epolkhov          #+#    #+#             */
/*   Updated: 2025/02/21 09:03:29 by alli             ###   ########.fr       */
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
	if (!client || client->allSet == false) {
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
			std::cout<< "Invalid channel name: "<< chan << ": it should start with #"<<std::endl;
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
				std::string msgJoin = ":" + client->getNickname() + "!" + client->getUsername() + "@" 
					+ client->getServerName()
					+ " JOIN " + newChannel->getName() + "\r\n";
				newChannel->sendToAll(msgJoin);
				newChannel->sendToAll(msg353);
			}
			else if (newChannel->isOperator(client) == true)
			{
				std::string modeMessage = ":" + client->getServerName() + 
					" MODE " + newChannel->getName() +
					" +o " + client->getNickname() + "\r\n";
				std::string msg353 = ":" + client->getServerName() + " 353 " + client->getNickname()
					+ newChannel->getName() + " @" + newChannel->getOperator(client)->getNickname()
					+ newChannel->getUsersNick() + "\r\n";
				std::string msgJoin = ":" + client->getNickname() + "!" + client->getUsername() + "@" 
					+ client->getServerName() + " JOIN " + newChannel->getName() + "\r\n"; 
				newChannel->sendToAll(msgJoin);
				newChannel->sendToAll(msg353);
			}
			std::string msg366 = ":" + client->getServerName() + " 366 " + client->getNickname() 
				+ " " + newChannel->getName() + "\r\n";
			newChannel->sendToAll(msg366);
			std::string topic = newChannel->getTopic();
			if (!topic.empty())
			{
				std::string topicMessage = ":" + client->getServerName() + " 332 " + client->getNickname() 
					+ " " + newChannel->getName() + " :" + "Welcome to our chat!" + "\r\n"
					+ ":" + client->getServerName() + " 333 " + client->getNickname() 
					+ " " + newChannel->getName() + " :" + topic + "\r\n";
				send(fd, topicMessage.c_str(), topicMessage.size(), 0);
			}
			count_joined++;
			
		}
	}
	return  0;
}

int Serv::cmdPART(int fd, std::vector<std::string> line)
{
	if (line.empty())
	{
		std::cerr << "Need more params" << std::endl;
		sendError(fd, "ERR_NEEDMOREARAMS :Need more params",  461);
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
			sendError(fd, "ERR_NOSUCHCHANNEL :No such channel",  403);
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
		sendError(fd, "ERR_NOSUCHCHANNEL :No such channel",  403);
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
			std::cerr << "No such channel" << std::endl;
			sendError(fd, "ERR_NOSUCHCHANNEL :No such channel",  403);
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
			std::cerr << "No channel" << std::endl;
			sendError(fd, "ERR_NOTONCHANNEL :No channel",  442);
        	continue;
		}
		if (!reason.empty()) {
			std::string message = ":" + client->getNickname() + "!" + client->getUsername() + "@"
				+ client->getServerName() + " PART " + channelName + " :" + reason + "\r\n";
			send_message(client->getFd(), message);
		} else {
			std::string message = ":" + client->getNickname() + "!" + client->getUsername() + "@"
				+ client->getServerName() + " PART " + channelName + "\r\n";
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
		sendError(fd, "ERR_NEEDMOREPARAMS :need more params",  461);
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
		sendError(fd, "ERR_NOSUCHCHANNEL :No such  channel",  403);
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
		std::cerr << "No suuch channel" << std::endl;
		sendError(fd, "ERR_NOSUCHCHANNEL :No such  channel",  403);
		return 1;
	}
	std::shared_ptr<Channel> channel = chanToFind->second;
	if (!channel->isUserInChannel(client))
	{
		std::cerr << "No suuch channel" << std::endl;
		sendError(fd, "ERR_NOSUCHCHANNEL :No such  channel",  403);
		return 1;
	}
	if (!channel->isOperator(client))
	{
		std::cerr << "Client is not an operator." << std::endl;
		sendError(fd, "ERR_CHANOPRIVSNEEDED :Client is not an operator.",  482);
		return 1;
	}
	Client* invitee = getClientByNickname(newUser);
	if (!invitee)
	{
		std::cout<< "User "<<newUser<< " not found."<< std::endl;
		return 1;
	}
	if (channel->isUserInChannel(invitee)) {
		std::cerr << "channel already in use." << std::endl;
		sendError(fd, "ERR_USERONCHANNEL :channel already in use",  443);
		return 1;
    }
	channel->addUser(invitee);
	invitee->joinChannel(channel);
	std::string rplInvitingMessage = ":" + client->getServerName() + " 341 " +
                                      client->getNickname() + " " +
                                      invitee->getNickname() + " " +
                                      channel->getName() + "\r\n";

    send_message(fd, rplInvitingMessage); 
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
		sendError(fd, "ERR_NEEDMOREPARAMS :need more params",  461);
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
		sendError(fd, "ERR_NOSUCHCHANNEL :No such  channel",  403);
		return 1;
	}
	auto findChan = _channels.find(chan);
	if (findChan == _channels.end())
	{
		std::cerr << "Nno suuch channel" << std::endl;
		sendError(fd, "ERR_NOSUCHCHANNEL :No such  channel",  403);
		return 1;
	}
	std::shared_ptr<Channel> channel = findChan->second;
	Client* client = getClientByFd(fd);
	if (!client) {
        std::cout << "Client not found for fd: " << fd << std::endl;
		sendError(fd, "ERR_NOSUCHCLIENT", 4);
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
				sendError(fd, "ERR_UNKNOWNMODE :Invalid mode", 472);
            	return 1;
        	}
			if (mode[i] == 'l' && (param.empty() || checkDigit(param) == 1))
			{
				std::cout <<"Mode set 'l' should contain only digits as a parameter and always require parameter."<< std::endl;
				sendError(fd, "ERR_LIMITEEXCEEDED :Mode 'l' requires a numeric parameter.", 472);
			}
			if (mode[i] == 'k' && param.empty())
			{
				std::cout<< "Parameter cannot be empty for the mode set 'k'"<<std::endl;
				sendError(fd, "ERR_KEYMISSING :Mode 'k' requires a parameter.", 472);
				return 1;
			}
			if (mode[i] == 'o' && (param.empty() || !channel->isUserInChannel(clientToAdd)))
			{
				std::cout<< "Parameter cannot be empty for the mode set 'o' or user is not in channel."<<std::endl;
				sendError(fd, "ERR_USERNOTINCHANNEL :Mode 'o' requires a valid user in the channel.", 472);
				return 1;
			}
			if (channel->isOperator(client))
			{
				if (mode[0] == '+' )
				{
					//std::cout<< "Mode + before "<< channel->isInviteOnly() <<std::endl;
					channel->setMode(mode[i], true, param, clientToAdd);
					//std::cout<< "Mode + After"<< channel->isInviteOnly() <<std::endl;
					//std::string message = "MODE " + channel->getName() + " " + mode[i];
					//channel->broadcastMessage(client->getNickname(), "MODE", message);
					 std::string modeMessage = ":" + client->getServerName() +
                                          " MODE " + channel->getName() +
                                          " " + mode[i] +"\r\n";
                	channel->sendToAll(modeMessage);
				}
				else if (mode[0] == '-')
				{
					//std::cout<< "Mode + before "<< channel->isInviteOnly() <<std::endl;
					channel->setMode(mode[i], false, param, clientToAdd);
					// std::string message = "MODE " + channel->getName() + " " + mode[i];
					// channel->broadcastMessage(client->getNickname(), "MODE", message);
					//std::cout<< "Mode + before "<< channel->isInviteOnly() <<std::endl;
					std::string modeMessage = ":" + client->getServerName() +
                                          " MODE " + channel->getName() +
                                          " " + mode[i]  + "\r\n";
                	channel->sendToAll(modeMessage);
				}
			}
			else{
				std::cerr << "Client is not an operator." << std::endl;
				sendError(fd, "ERR_NOPRIVILEGES : You do not have permission to change the mode.", 481);
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
		sendError(fd, "ERR_NEEDMOREPARAMS", 461);
		return 1;
	}
	std::string checkChan = line[0];
	if (checkChan[0] != '#' || checkChanName(checkChan) == 1)
	{
		sendError(fd, "ERR_NOSUCHCHANNEL", 403);
		return 1;
	}
	auto findChan = _channels.find(checkChan);
	if (findChan == _channels.end())
	{
		sendError(fd, "ERR_NOSUCHCHANNEL :No such  channel",  403);
		return 1;
	}
	std::shared_ptr<Channel> channel = findChan->second;
	Client* client = getClientByFd(fd);
	std::vector<std::string> reason;
	if (!client) {
		sendError(fd, "ERR_NOTONCHANNEL", 442);
        return 1;
    }
	std::vector<std::string> usersToKick;
	std::string message;
	for (size_t i = 1; i < line.size(); ++i)
	{
		Client* userKick = getClientByNickname(line[i]);
		if (!userKick || !channel->isUserInChannel(userKick))
		{
			sendError(fd, "ERR_USERNOTINCHANNEL", 441);
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
		else {
			std::cout << "not chan operator" << std::endl;
			sendError(fd, "ERR_CHANOPRIVSNEEDED", 482);
            return 1;
		}	
	}
	std::string fullReason;
    if (!reason.empty())
    {
		for (size_t i = 0; i < reason.size(); ++i)
        {
            fullReason += reason[i];
            if (i < reason.size() - 1) // Add space between words if it's not the last one
            {
                fullReason += " ";
            }
        }
    }
	for (size_t i = 0; i < usersToKick.size(); i++)
	{
		message = ":" + client->getNickname() + "!" + client->getUsername() + "@" 
			+ client->getServerName() + " KICK " + channel->getName() + " " + usersToKick[i];
		if (!fullReason.empty())
		{
			message += " (" + fullReason + ")";
		}
		ssize_t bytesSent = send(fd, message.c_str(), message.size(), 0);
		if (bytesSent == -1) {
			std::cerr << "Error sending TOPIC response to client " << fd << std::endl;
		}
	}
	return 0;
}


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
		std::cerr << "Nno suuch channel" << std::endl;
		sendError(fd, "ERR_NOSUCHCHANNEL :No such  channel",  403);
		return 1;
	}
	std::shared_ptr<Channel> channel = findChan->second;
	Client* client = getClientByFd(fd);
	std::vector<std::string> reason;
	if (!client) {
		std::cerr << "Client not found." << std::endl;
		sendError(fd, "ERR_NOTONCHANNEL :Client not found.",  442);
		return 1;
    }
	if (!channel->isUserInChannel(client)) {
		std::cerr << "Client not found." << std::endl;
		sendError(fd, "ERR_NOTONCHANNEL :Client not found.",  442);
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
		if (channel->setTopic(topic, client) == false)
			return 1;
		std::string broadcastMessage = ":" + client->getNickname() + "!" + client->getUsername() + "@" 
			+ client->getServerName() + " TOPIC " + channel->getName() + " :" + topic + "\r\n";
		ssize_t bytesSent = send(fd, broadcastMessage.c_str(), broadcastMessage.size(), 0);
			if (bytesSent == -1) {
				std::cerr << "Error sending TOPIC response to client " << fd << std::endl;
			}
	}
	else{
		std::string currentTopic = channel->getTopic();
        std::string topicResponse;

        if (currentTopic.empty()) {
            topicResponse = ":" + client->getServerName() + " 331 " + client->getNickname() 
			+ " " + channel->getName() + " :No topic is set\r\n";
        }
        else {
            topicResponse = ":" + client->getServerName() + " 332 " + client->getNickname() 
				+ " " + channel->getName() + " :" + "Welcome to our chat!" + "\r\n"
				+ ":" + client->getServerName() + " 333 " + client->getNickname() 
				+ " " + channel->getName() + " :" + currentTopic + "\r\n";
        }

        ssize_t bytesSent = send(fd, topicResponse.c_str(), topicResponse.size(), 0);
        if (bytesSent == -1) {
            std::cerr << "Error sending TOPIC response to client " << fd << std::endl;
        }
	}
	return 0;
}