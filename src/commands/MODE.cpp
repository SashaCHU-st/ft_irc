/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MODE.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: epolkhov <epolkhov@student.42.fr>          #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-02-24 17:25:05 by epolkhov          #+#    #+#             */
/*   Updated: 2025-02-24 17:25:05 by epolkhov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Serv.hpp"

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