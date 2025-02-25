/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TOPIC.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: epolkhov <epolkhov@student.42.fr>          #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-02-24 17:28:01 by epolkhov          #+#    #+#             */
/*   Updated: 2025-02-24 17:28:01 by epolkhov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Serv.hpp"

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