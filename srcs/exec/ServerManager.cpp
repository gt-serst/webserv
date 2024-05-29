/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:04:51 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/29 15:54:31 by gt-serst         ###   ########.fr       */
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

ServerManager::ServerManager(void){}

ServerManager::~ServerManager(void){

	_servers.clear();
	_sockets.clear();
	_ready.clear();
}

void	ServerManager::launchServer(t_server_scope *servers, int nb_servers){

	initServer(servers, nb_servers);
	serverRoutine();
	clear();
}

void	ServerManager::initServer(t_server_scope *servers, int nb_servers){

	FD_ZERO(&_fd_set);
	for (size_t i = 0; i < nb_servers; i++)
	{
		int		fd;
		Server	server(servers[i]);

		server.createServerSocket();
		fd = server.getFd();
		FD_SET(fd, &_fd_set);
		_servers.insert(std::make_pair(fd, server));
	}
}

void	ServerManager::serverRoutine(void){

	while (1)
	{
		std::cout << "New loop" << std::endl;
		int				rc;
		fd_set			reading_set;
		fd_set			writing_set;
		struct timeval	timeout;

		timeout.tv_sec  = 1;
		timeout.tv_usec = 0;
		ft_memcpy(&reading_set, &_fd_set, sizeof(_fd_set));
		FD_ZERO(&writing_set);
		for (std::vector<int>::iterator it = _ready.begin(); it < _ready.end(); ++it)
			FD_SET(*it, &writing_set);

		rc = select(FD_SETSIZE, &reading_set, &writing_set, NULL, &timeout);
		if (rc > 0)
		{
			for (std::vector<int>::iterator it = _ready.begin(); it < _ready.end(); ++it)
			{
				if (FD_ISSET(*it, &writing_set))
				{
					rc = _sockets[*it]->sendResponse(*it);
					if (rc == 0)
						_ready.erase(it);
					else if (rc == -1)
					{
						FD_CLR(*it, &_fd_set);
						FD_CLR(*it, &reading_set);
						_sockets.erase(*it);
						_ready.erase(it);
					}
					break;
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
					else if (rc == -1)
					{
						FD_CLR(*it, &_fd_set);
						FD_CLR(*it, &reading_set);
						_sockets.erase(it->first);
					}
					break;
				}
			}
			for (std::map<int, Server>::iterator it = _servers.begin(); it < _servers.end(); ++it)
			{
				if (FD_ISSET(it->first, &reading_set))
				{
					int client_fd = it->second.listenClientConnection();

					if (client_fd != -1)
					{
						FD_SET(client_fd, &_fd_set);
						_sockets.insert(std::make_pair(client_fd, it->second));
					}
					break;
				}
			}
		}
		else
			perror("Select() failed");

		std::cout << "End loop" << std::endl;
	}
}

void	ServerManager::clear(void){

	for (std::map<int, Server>::iterator it = _servers.begin(); it < _servers.end(); ++it)
		it->second.closeServerSocket();
}
