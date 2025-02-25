/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   INVITE.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: epolkhov <epolkhov@student.42.fr>          #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-02-24 17:23:46 by epolkhov          #+#    #+#             */
/*   Updated: 2025-02-24 17:23:46 by epolkhov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
 
#include "../Serv.hpp"

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
