/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:04:51 by gt-serst          #+#    #+#             */
/*   Updated: 2024/06/04 18:46:25 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include "Server.hpp"
#include "../parser/confParser.hpp"
#include <cstring>
#include <map>
#include <vector>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

#include <iostream>
#include <stdio.h>

ServerManager::ServerManager(void){

	//std::cout << "ServerManager created" << std::endl;
}

ServerManager::~ServerManager(void){

	// _servers.clear();
	// _sockets.clear();
	// _ready.clear();
	//std::cout << "ServerManager destroyed" << std::endl;
}

void	ServerManager::launchServer(t_server_scope *servers, int nb_servers){

	initServer(servers, nb_servers);
	serverRoutine();
	clear();
}

void	ServerManager::initServer(t_server_scope *servers, int nb_servers){

	FD_ZERO(&_fd_set);
	for (int i = 0; i < nb_servers; i++)
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
		//std::cout << "New loop" << std::endl;
		int				rc;
		fd_set			reading_set;
		fd_set			writing_set;
		struct timeval	timeout;

		rc = 0;
		while (rc == 0)
		{
			timeout.tv_sec  = 1;
			timeout.tv_usec = 0;
			std::memcpy(&reading_set, &_fd_set, sizeof(_fd_set));
			FD_ZERO(&writing_set);
			for (std::vector<int>::iterator it = _ready.begin(); it < _ready.end(); ++it)
				FD_SET(*it, &writing_set);

			rc = select(FD_SETSIZE, &reading_set, &writing_set, NULL, &timeout);
		}
		if (rc > 0)
		{
			for (std::vector<int>::iterator it = _ready.begin(); it != _ready.end(); ++it)
			{
				//std::cout << "First loop: Send" << std::endl;
				if (FD_ISSET(*it, &writing_set))
				{
					//std::cout << "Entering first loop" << std::endl;
					int rc = _sockets[*it]->sendResponse(*it);
					if (rc == 0)
					{
						_sockets.erase(*it);
						_ready.erase(it);
					}
					else if (rc == -1)
					{
						FD_CLR(*it, &_fd_set);
						FD_CLR(*it, &reading_set);
						_sockets.erase(*it);
						_ready.erase(it);
					}
					rc = 0;
					break;
				}
			}
			for (std::map<int, Server*>::iterator it = _sockets.begin(); it != _sockets.end(); ++it)
			{
				//std::cout << "Second loop: Read" << std::endl;
				if (FD_ISSET(it->first, &reading_set))
				{
					//std::cout << "Entering second loop" << std::endl;
					int rc = it->second->readClientSocket(it->first);
					if (rc == 0)
					{
						rc = it->second->handleRequest(it->first);
						if (rc == 0)
							_ready.push_back(it->first);
					}
					else if (rc == -1)
					{
						FD_CLR(it->first, &_fd_set);
						FD_CLR(it->first, &reading_set);
						_sockets.erase(it->first);
						//it = _sockets.begin();
					}
					rc = 0;
					break;
				}
			}
			for (std::map<int, Server>::iterator it = _servers.begin(); it != _servers.end(); ++it)
			{
				//std::cout << "Third loop: Listen" << std::endl;
				if (FD_ISSET(it->first, &reading_set))
				{
					//std::cout << "Entering third loop" << std::endl;
					int client_fd = it->second.listenClientConnection();

					if (client_fd != -1)
					{
						FD_SET(client_fd, &_fd_set);
						_sockets.insert(std::make_pair(client_fd, &(it->second)));
					}
					rc = 0;
					break;
				}
			}
		}
		/*else
			perror("Select() failed");*/
		else
		{
			for (std::map<int, Server*>::iterator it = _sockets.begin() ; it != _sockets.end() ; it++)
			{
				//std::cout << "Four loop: Close" << std::endl;
				it->second->closeClientSocket(it->first);
			}
			_sockets.clear();
			_ready.clear();
			FD_ZERO(&_fd_set);
			for (std::map<int, Server>::iterator it = _servers.begin() ; it != _servers.end() ; it++)
			{
				//std::cout << "Set a server" << std::endl;
				FD_SET(it->first, &_fd_set);
			}
		}
		//std::cout << "End loop" << std::endl;
	}
}

void	ServerManager::clear(void){

	for (std::map<int, Server>::iterator it = _servers.begin(); it != _servers.end(); ++it)
		it->second.closeServerSocket();
}
