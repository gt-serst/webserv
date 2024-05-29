/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:04:51 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/29 13:20:52 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include "Server.hpp"
#include "../parser/confParser.hpp"
#include <map>
#include <vector>
#include <sys/select.h>
#include <sys/time.h>

#include <iostream>
#include <stdio.h>

ServerManager::ServerManager(){}

ServerManager::~ServerManager(){}

void	ServerManager::launchServer(t_server_scope *servers, int nb_servers){

	for (size_t i = 0; i < nb_servers; i++)
	{
		_servers.push_back(Server(servers[i]));
		_servers[i].createServerSocket();
	}
	setUp();
	serverRoutine();
	for (size_t i = 0; i < _servers.size(); i++)
		_servers[i].closeServerSocket();
}

void	ServerManager::setUp(){

	
}

void	ServerManager::serverRoutine(void){

	int				rc;
	fd_set			ready_sockets;
	fd_set			current_sockets;
	struct timeval	timeout;

	while (1)
	{
		std::cout << "New loop" << std::endl;

		timeout.tv_sec  = 1;
		timeout.tv_usec = 0;
		ft_memcpy(&reading_set, &fd_set, sizeof(_fd_set));
		FD_ZERO(&writing_set);
		for (std::vector<int>::iterator it = _ready.begin; it < _ready.end(); ++it)
			FD_SET(*it, &writing_set);

		rc = select(FD_SETSIZE, &reading_set, &writing_set, NULL, &timeout);
		if (rc > 0)
		{
			for (std::vector<int>::iterator it = _ready.begin; it < _ready.end(); ++it)
			{
				if (FD_ISSET(*it, &writing_set))
				{
					_sockets[*it]->sendResponse(*it);
					_ready.erase(it);
				}
			}
			for (std::map<int, Server>::iterator it = _sockets.begin(); it < _sockets.end(); ++it)
			{
				if (FD_ISSET(it->first, &reading_set))
				{
					rc = it->second.readClientSocket(it->first);
					if (rc == 0)
					{
						it->second.handleRequest(it->first);
						_ready.push_back(it->first);
					}
				}
			}
			for (std::map<int, Server>::iterator it = _servers.begin(); it < _servers.end(); ++it)
			{
				if (FD_ISSET(it->first, &reading_set))
				{
					int client_fd = it->second.listenClientConnection();

					if (client_fd != -1)
					{
						FD_SET(client_fd, &writing_set);
						_sockets.insert(std::make_pair(client_fd, &(it->second)));
					}
				}
			}
		}
		else
			perror("Select() failed");

		std::cout << "End loop" << std::endl;
	}
}
