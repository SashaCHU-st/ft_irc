/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelCMDs.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alli <alli@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 12:59:50 by epolkhov          #+#    #+#             */
/*   Updated: 2025/02/21 14:49:28 by alli             ###   ########.fr       */
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
		//std::cout<<"No parameter after JOIN"<< std::endl;
		//sendError(fd, "ERR_NEEDMOREPARAMS" , 461);
		sendError(fd, "JOIN :Not enough parameters", 461);
		return 1;
	}
	if (line.size() > 2)
	{
		//std::cerr<< "Too many parameters for the JOIN command."<< std::endl;
		//sendError(fd, "ERR_TOOMANYPARAMS" , 461);
		sendError(fd, "JOIN :Too many parameters", 461);
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
        //std::cerr << "Client not found for fd: " << fd << std::endl;
		sendError(fd, "ERR_NOSUCHCLIENT", 4);
        return 1;
    }
	if (channels.size() != keys.size() && keys.size() != 1) {
        //std::cerr << "Number of keys doesn't match the number of channels." << std::endl;
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
			//std::cerr<< "Invalid channel name: "<< chan<< ": it should start with #"<<std::endl;
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
		
			//std::cout << userList << std::endl;
			newChannel->sendToAll(modeMessage);
			newChannel->sendToAll(msgJoin);
			newChannel->sendToAll(msg353);
			std::string msg366 = ":" + client->getServerName() + " 366 " + client->getNickname() 
								+ " " + newChannel->getName() + "\r\n";
			newChannel->sendToAll(msg366);
			count_joined++;
		}
		else
		{
			//std::cout<< "Channel: "<< chan <<" already exists." << std::endl;
			newChannel = findChan->second;
			if (!newChannel->getPassword().empty())
			{
				if (keys[i].empty())
				{
					//sendError(fd, "ERR_BADCHANNELKEY" + client->getNickname() + " " + newChannel->getName() + " :Cannot join channel (+k) ", 475);
					sendError(fd, newChannel->getName() + " :Cannot join channel (+k)", 475);
					continue ;
				}
				if (!keys[i].empty() && !newChannel->checkPassword(keys[i]))
				{
					//std::cerr << "Incorrect password for channel: " << chan << std::endl;
					//sendError(fd, "ERR_BADCHANNELKEY" + client->getNickname() + " " + newChannel->getName() + " :Incorrect password (+k) ", 475);
					sendError(fd, newChannel->getName() + " :Cannot join channel (+k)", 475);
					continue ;
				}
			}
			if (newChannel->isUserInChannel(client)) {
				//std::cout << "User " << client<< " is already in the channel: "<< newChannel->getName() << std::endl;
				//sendError(fd, "ERR_USER_ALREADY_IN_CHANNEL " + client->getNickname() + " " + newChannel->getName() + " :You are already in the channel", 433);
				sendError(fd, newChannel->getName() + " :You are already in the channel", 433);
				continue ;
			}
			if (newChannel->getUserLimit() == newChannel->getUserCount())
			{
				//std::cout<< "User "<< client->getNickname() << " cannot join the channel as the ammount of users are limited."<< std::endl;
				//sendError(fd, "ERR_CHANNELISFULL" + client->getNickname() + " " + newChannel->getName() + " :Cannot join channel (+l)", 471);
				sendError(fd, newChannel->getName() + " :Cannot join channel (+l)", 471);
				continue ;
			}
			if (newChannel->isInviteOnly())
			{
				if (client->isInvitedToChan() == false)
				{
					//std::cout<< "User "<< client->getNickname()<< " tries to join the channel and wait for INVITE." << std::endl;
					//sendError(fd, "ERR_INVITEONLYCHAN " + newChannel->getName() + " :Cannot join channel (+i)", 473);
					sendError(fd, newChannel->getName() + " :Cannot join channel (+i)", 473);
					continue ;
				}
			}		
			newChannel->addUser(client);
			client->joinChannel(newChannel);
			if (newChannel->isOperator(client) == false)
			{
				std::string msg353 = ":" + client->getServerName() + " 353 " + client->getNickname() 
								+ " = " + newChannel->getName() + " :" + newChannel->getUsersNick() + "\r\n";
				//std::cout << "msg353 3:" << msg353 << std::endl;
				std::string msgJoin = ":" + client->getNickname() + "!" + client->getUsername() + "@" 
					+ client->getServerName()
					+ " JOIN " + newChannel->getName() + "\r\n"; 
				std::cout << "msgJoin2: " << msgJoin << std::endl;
				newChannel->sendToAll(msgJoin);
				newChannel->sendToAll(msg353);
			}
			else //if (newChannel->isOperator(client) == true)
			{
				std::string modeMessage = ":" + client->getServerName() +
					" MODE " + newChannel->getName() +
					" +o " + client->getNickname() + "\r\n";
				std::string msg353 = ":" + client->getServerName() + " 353 " + client->getNickname()
					+ newChannel->getName() + " @" + newChannel->getOperator(client)->getNickname()
					+ newChannel->getUsersNick() + "\r\n";
				//std::cout << "msg353 4:" << msg353 << std::endl;
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
				std::string topicMessage = ":ircserver 332 " + client->getNickname() + " " + newChannel->getName() + " :Welcome to " + topic + "\r\n";
                std::cout << topicMessage << std::endl;
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
		//std::cerr << "Need more params" << std::endl;
		sendError(fd, "PART :Need more params",  461);
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
            	//std::cout << "Invalid channel name: " << channel << std::endl;
				sendError(fd, channel + " :No such channel", 403);
            	//return 1;
				continue ;
        	}
		}
	}
	if (chanCheck[0] == '#' && checkChanName(chanCheck) != 1)
	{
		channels.push_back(chanCheck);
	}
	else
	{
		//std::cerr << "No such channel" << std::endl;
		sendError(fd, "ERR_NOSUCHCHANNEL :No such channel",  403);
		return 1;
	}
	std::string reason;
	if (line.size() > 1)
	{
		reason = "";
    	for (size_t i = 1; i < line.size(); ++i)
		{
        	reason += line[i];
        	if (i != line.size() - 1) 
			{
           		reason += " ";
        	}
    	}
	}
	for (const std::string& channelName : channels)
	{
		auto it = _channels.find(channelName);
		if (it == _channels.end())
		{
			//std::cerr << "No such channel" << std::endl;
			sendError(fd, "ERR_NOSUCHCHANNEL :No such channel",  403);
			continue;
		}
		std::shared_ptr<Channel> channel = it->second;

		Client* client = getClientByFd(fd);
		if (!client) {
            //std::cout << "Client not found by fd: " << fd << std::endl;
			sendError(fd, "ERR_NOSUCHCLIENT", 4);
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
                	srand(time(0));
                	int randomIndex = rand() % usersInChannel.size();
               		Client* randomUser = usersInChannel[randomIndex];
                	channel->addOperator(randomUser);
					std::string modeMessage = ":" + client->getServerName() + " MODE " + channelName +
                                          " +o " + randomUser->getNickname() + "\r\n";
                	channel->sendToAll(modeMessage);
                	//std::cout << "Assigned user " << randomUser->getNickname() << " as operator in channel " << channelName << std::endl;
            	}
			}
			else{
				channel->removeUser(client);
				client->leaveChannel(channel->getName());
			}
		}
		else
		{
			//std::cerr << "No channel" << std::endl;
			sendError(fd, "ERR_NOTONCHANNEL :No channel",  442);
        	continue;
		}
		if (!reason.empty()) {
			std::string message = ":" + client->getNickname() + "!" + client->getUsername() + "@"
				+ client->getServerName() + " PART " + channelName + " :" + reason + "\r\n";
			send_message(client->getFd(), message);
			channel->sendToAll(message);
		} else {
			std::string message = ":" + client->getNickname() + "!" + client->getUsername() + "@"
				+ client->getServerName() + " PART " + channelName + "\r\n";
			send_message(client->getFd(), message);
			channel->sendToAll(message);
		}
	}
	return 0;
}

int Serv::cmdINVITE(int fd, std::vector<std::string> line)
{
	if (line.empty())
	{
		//std::cerr << "Need more params" << std::endl;
		sendError(fd, "ERR_NEEDMOREPARAMS :need more params",  461);
		return 1;
	}
	if (line.size() != 2)
	{
		std::cerr<< "Too many parameters for the INVITE command."<< std::endl;
		return 1;
	}
	std::string newUser = line[0];
	std::string chanToAdd =line[1];
	if (chanToAdd[0] != '#' || chanToAdd.empty() || checkChanName(chanToAdd) == 1)
	{
		//std::cout<< "Invalid channel name."<<std::endl;
		sendError(fd, "ERR_NOSUCHCHANNEL :No such  channel",  403);
		return 1;
	}
	Client* client = getClientByFd(fd);
	if (!client) {
        //std::cerr << "Client not found for fd: " << fd << std::endl;
		sendError(fd, "ERR_NOSUCHCLIENT", 4);
        return 1;
    }
	auto chanToFind = _channels.find(chanToAdd);
	if (chanToFind == _channels.end())
	{
		//std::cerr << "No suuch channel" << std::endl;
		sendError(fd, "ERR_NOSUCHCHANNEL :No such  channel",  403);
		return 1;
	}
	std::shared_ptr<Channel> channel = chanToFind->second;
	if (!channel->isUserInChannel(client))
	{
		//std::cerr << "No suuch channel" << std::endl;
		sendError(fd, "ERR_NOSUCHCHANNEL :No such  channel",  403);
		return 1;
	}
	if (!channel->isOperator(client))
	{
		//std::cerr << "Client is not an operator." << std::endl;
		sendError(fd, "ERR_CHANOPRIVSNEEDED :Client is not an operator.",  482);
		return 1;
	}
	Client* invitee = getClientByNickname(newUser);
	if (!invitee)
	{
		//std::cout<< "User "<<newUser<< " not found."<< std::endl;
		sendError(fd, "ERR_NOSUCHCLIENT", 4);
		return 1;
	}
	if (channel->isUserInChannel(invitee)) {
		//std::cerr << "channel already in use." << std::endl;
		sendError(fd, "ERR_USERONCHANNEL :channel already in use",  443);
		return 1;
    }
	std::string rplInvitingMessage = ":" + client->getServerName() + " 341 " +
                                      client->getNickname() + " " +
                                      invitee->getNickname() + " " +
                                      channel->getName() + "\r\n";

    send_message(fd, rplInvitingMessage);
	std::string inviteeMessage = ":" + client->getServerName() + " INVITE " +
                                 invitee->getNickname() + " " + channel->getName() + "\r\n";

    send_message(invitee->getFd(), inviteeMessage);
	invitee->setInvitedToChannel(true);
	//std::cout << "User " << newUser << " successfully invited to channel " << chanToAdd << "." << std::endl;
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
	if (str.empty())
		return 1;
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
	if (line.empty() || line.size() > 3)
	{
		//std::cout << "Invalid number of parameters for MODE command."<< std::endl;
		sendError(fd, "ERR_NEEDMOREPARAMS :need more params",  461);
		return 1;
	}
	Client* client = getClientByFd(fd);
	if (!client) {
        //std::cout << "Client not found for fd: " << fd << std::endl;
		sendError(fd, "ERR_NOSUCHCLIENT", 4);
        return 1;
    }
	std::string chan = line[0];
	if (line.size() == 1)
	{
		std::string message = ":" + client->getServerName() + " " + " 324 " 
					+ client->getNickname() + " " + chan + "\r\n";
	
    	if (send(client->getFd(), message.c_str(), message.size(), 0) == -1)
			std::cerr << "not sent" << std::endl;
		return 0;
	}
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
		//std::cerr<<"Invalid channel name."<< std::endl;
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
	Client* clientToAdd = getClientByNickname(param);
	for (size_t i = 0; i < mode.size() ; i++)
	{
		if (mode[i] == '+' || mode[i] == '-')
			i++;
		if (checkValidMode(mode[i]) == 0) {
            //std::cerr << "Invalid mode character." << std::endl;
			sendError(fd, "ERR_UNKNOWNMODE :Invalid mode", 472);
            return 1;
        }
		if (mode[i] == 'l')
		{
			if (mode[0] == '-' && param.empty())
				std::cout<< "Unset limit for channel"<< std::endl;
			if (mode[0] == '+' && (param.empty() || checkDigit(param) == 1))
			{
				//std::cout <<"Mode set 'l' should contaiways require parametn only digits as a parameter and aler."<< std::endl;
				sendError(fd, "ERR_LIMITEEXCEEDED : Mode 'l' requires a numeric parameter.", 472);
				return 1;
			}
		}
		if (mode[i] == 'k' && param.empty())
		{
			//std::cerr<< "Parameter cannot be empty for the mode set 'k'"<<std::endl;
			sendError(fd, "ERR_KEYMISSING :Mode 'k' requires a parameter.", 472);
			return 1;
		}
		if (mode[i] == 'o' && (param.empty() || !channel->isUserInChannel(clientToAdd)))
		{
			//std::cerr<< "Parameter cannot be empty for the mode set 'o' or user is not in channel."<<std::endl;
			sendError(fd, "ERR_USERNOTINCHANNEL :Mode 'o' requires a valid user in the channel.", 472);
			return 1;
		}
		if (channel->isOperator(client))
		{
			if (mode[0] == '+' )
			{
				if (mode[i] == 'o')
				{
					channel->setMode(mode[i], true, param, clientToAdd);
					std::string modeMessage = ":" + client->getServerName() +
                                              " MODE " + channel->getName() +
                                              " +o " + clientToAdd->getNickname() + "\r\n";
					std::string msg353 = ":" + client->getServerName() + " 353 " + client->getNickname()
							+ channel->getName() + " @" + channel->getOperator(client)->getNickname()
								+ channel->getUsersNick() + "\r\n";
						//std::cout << "msg353 4:" << msg353 << std::endl;
					channel->sendToAll(modeMessage);
					channel->sendToAll(msg353);
				}
				else
				{
					channel->setMode(mode[i], true, param, clientToAdd);
					std::string modeMessage = ":" + client->getServerName() +
                                		 " MODE " + channel->getName() +
                                          " " + mode[i] +"\r\n";
					channel->sendToAll(modeMessage);
				}
			}
			else if (mode[0] == '-')
			{
				if (mode[i] == 'o')
				{
					channel->setMode(mode[i], false, param, clientToAdd);
					std::string modeMessage = ":" + client->getServerName() +
                                              " MODE " + channel->getName() +
                                              " -o " + clientToAdd->getNickname() + "\r\n";
					std::string msg353 = ":" + client->getServerName() + " 353 " + client->getNickname()
								+ channel->getName() + " " + channel->getOperator(client)->getNickname()
								+ channel->getUsersNick() + "\r\n";
					channel->sendToAll(modeMessage);
					channel->sendToAll(msg353);
				}
				else
				{
					channel->setMode(mode[i], false, param, clientToAdd);
					std::string modeMessage = ":" + client->getServerName() +
											" MODE " + channel->getName() +
											" " + mode[i]  + "\r\n";
					channel->sendToAll(modeMessage);

				}
			}
		}
		else{
			//std::cerr << "Client is not an operator." << std::endl;
			sendError(fd, "ERR_NOPRIVILEGES : You do not have permission to change the mode.", 481);
    		return 1;
		}	
	}
	return 0;
}


int Serv::cmdKICK(int fd, std::vector<std::string> line)
{
    std::cout << "Line size: " << line.size() << std::endl;
    if (line.empty() || line.size() < 2)
    {
		sendError(fd, "KICK :Not enough parameters", 461);
        return 1;
    }

    std::string checkChan = line[0];
    if (checkChan[0] != '#' || checkChanName(checkChan) == 1)
    {
		sendError(fd, checkChan + " :Channel call is incorrect", 403);
        return 1;
    }

    auto findChan = _channels.find(checkChan);
    if (findChan == _channels.end())
    {
		sendError(fd, checkChan + " :No such channel", 403);
        return 1;
    }

    std::shared_ptr<Channel> channel = findChan->second;
    Client* client = getClientByFd(fd);
    if (!client)
	{
		sendError(fd, checkChan + " :You're not on that channel", 442);
        return 1;
    }
	if (!channel->isOperator(client))
    {
        sendError(fd, checkChan + " :You're not a channel operator", 482);
        return 1;
    }

    std::string reason;
	size_t reasonIndx = line.size();
    std::string message = "";

	for (size_t i = 1; i < line.size(); ++i)
	{
		if (line[i][0] == ':')
		{
			reasonIndx = i;
			break;	
		}
	}
	if (reasonIndx < line.size())
	{
    	reason = line[reasonIndx].substr(1);
    	for (size_t i = reasonIndx + 1; i < line.size(); ++i)
   		{
        	reason += " " + line[i]; 
    	}
	}
		
    for (size_t i = 1; i < reasonIndx; ++i)
    {
		Client* userKick = getClientByNickname(line[i]);
        if (!userKick || !channel->isUserInChannel(userKick))
        {
            //sendError(fd, "ERR_USERNOTINCHANNEL", 441);
			std::cout << "Is it not user in channel?"<< std::endl;
			sendError(fd, line[i] + " " + checkChan + " :User is not on that channel", 441);
            continue;
        }
		std::string kickMessage = ":" + client->getServerName() + " KICK " 
                    + channel->getName() + " " + userKick->getNickname() + "\r\n";

		send(userKick->getFd(), kickMessage.c_str(), kickMessage.size(), 0);

		message = ":" + client->getNickname() + "!" + client->getUsername() + "@" 
        			+ client->getServerName() + " KICK " + channel->getName() + " " + userKick->getNickname();
		if (!reason.empty())
		{
			message += " :" + reason;
		}
		message += "\r\n";
		channel->sendToAll(message);  
        channel->removeUser(userKick);
        userKick->leaveChannel(channel->getName());
		//std::cout << "KICK message sent to client " << userKick->getNickname() << ": " << kickMessage << std::endl;
    }
    return 0;
}


int Serv::cmdTOPIC(int fd, std::vector<std::string> line)
{
	if (line.empty())
	{
		//std::cerr << "Invalid number of parameters for TOPIC command."<< std::endl;
		sendError(fd, "TOPIC :Not enough parameters", 461);
		return 1;
	}
	std::string checkChan = line[0];
	if (checkChan[0] != '#' || checkChanName(checkChan) == 1)
	{
		//std::cerr<< "Invalid channel name."<<std::endl;
		sendError(fd, checkChan + " :Channel call is incorrect", 403);
		return 1;
	}
	auto findChan = _channels.find(checkChan);
	if (findChan == _channels.end())
	{	
		//std::cerr << "Nno suuch channel" << std::endl;
		sendError(fd, "ERR_NOSUCHCHANNEL :No such  channel",  403);
		return 1;
	}
	std::shared_ptr<Channel> channel = findChan->second;
	Client* client = getClientByFd(fd);
	std::vector<std::string> reason;
	if (!client) {
		//std::cerr << "Client not found." << std::endl;
		sendError(fd, "ERR_NOTONCHANNEL :Client not found.",  442);
		return 1;
    }
	if (!channel->isUserInChannel(client)) {
		//std::cerr << "Client not found." << std::endl;
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
		channel->sendToAll(broadcastMessage);
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
		channel->sendToAll(topicResponse);
	}
	return 0;
}