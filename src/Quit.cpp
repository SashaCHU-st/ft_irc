#include "Serv.hpp"

int Serv::quit(int fd)
{
    if(clients[fd].getFd() == fd)
    {
        auto channels = clients[fd].getJoinedChannels();
        for (size_t i = 0; i < channels.size(); i++)
        {
            auto client = getClientByFd(fd);
            auto channel = channels[i];
            if (channel->isOperator(client) == true)
            {
                channel->removeOperator(client);
                channel->removeUser(client);
                if (!channel->getUsers().empty())
                {
                    std::vector<Client*> usersInChannel = channel->getUsers();
                    srand(time(0));
                    int randomIndex = rand() % usersInChannel.size();
                    Client* randomUser = usersInChannel[randomIndex];
                    channel->addOperator(randomUser);
                    std::string modeMessage = ":" + client->getServerName() + " MODE " + channel->getName() +
                                " +o " + randomUser->getNickname() + "\r\n";
                    channel->sendToAll(modeMessage);
                }
            std::string quitMsg = ":" + client->getNickname() + "!" + client->getUsername()
                        + "@" + client->getHostName()+ " QUIT :Lost terminal\r\n";
            channel->sendToAll(quitMsg);
            }
            else
            {
                channel->removeUser(client);
                std::string quitMsg = ":" + client->getNickname() + "!" + client->getUsername()
                            + "@" + client->getHostName()+ " QUIT :Lost terminal\r\n";
                channel->sendToAll(quitMsg);
            }
        }
        clients.erase(fd);
        close (fd);
        return(0);
    }
    return 1;
}
