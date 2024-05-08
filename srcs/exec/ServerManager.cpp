/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:04:51 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/08 17:51:24 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include <vector>
#include <algorithm>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <curses.h>
#include <poll.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

ServerManager::ServerManager() : _nfds(0){}

ServerManager::~ServerManager(){}

void	ServerManager::launchServers(void){

	Server s1(8080);
	_servers.push_back(s1);
	createSockets();
	checkSockets();
	closeSockets();
}

void	ServerManager::createSockets(void){

	int	rc;
	
	memset(_fds, 0, sizeof(_fds));
	for (int i = 0; i < _servers.size(); i++)
	{
		_servers[i]._server_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (_servers[i]._server_fd < 0)
		{
			perror("socket() failed");
			exit(-1);
		}
		int reuse = 1;
		rc = setsockopt(_servers[i]._server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
		if (rc < 0)
		{
			perror("setsockopt() failed");
			exit(-1);
		}
		rc = fcntl(_servers[i]._server_fd, F_SETFL, O_NONBLOCK);
		if (rc < 0)
		{
			perror("fcntl() failed");
			exit(-1);
		}
		_servers[i]._server_addr.sin_family = AF_INET;
		_servers[i]._server_addr.sin_addr.s_addr = INADDR_ANY;
		_servers[i]._server_addr.sin_port = htons(_servers[i].getPort());
		_servers[i]._server_addr_len = sizeof(_servers[i]._server_addr);
		rc = bind(_servers[i]._server_fd, (struct sockaddr *) &(_servers[i]._server_addr), _servers[i]._server_addr_len);
		if (rc < 0)
		{
			perror("bind() failed");
			exit(-1);
		}
		int connection_backlog = 5;
		rc = listen(_servers[i]._server_fd, connection_backlog);
		if (rc < 0)
		{
			perror("listen() failed");
			exit(-1);
		}
		_fds[i].fd = _servers[i]._server_fd;
		_fds[i].events = POLLIN;
		_nfds++;
	}
}

void	ServerManager::checkSockets(void){

	int	timeout;
	int	rc;

	timeout = (3 * 60 * 1000);
	while (1)
	{
		rc = poll(_fds, _nfds, timeout);
		if (rc < 0)
		{
			perror("poll() failed");
			exit(-1);
		}
		if (rc == 0)
		{
			perror("poll() time limit expires");
			exit(-1);
		}
		for (int i = 0; i < _nfds; i++)
		{
			Client client;

			client._client_addr_len = sizeof(client._client_addr);
			if (!(_fds[i].revents & POLLIN))
				continue;
			if (_fds[i].fd == _servers[i]._server_fd)
			{
				client._client_fd = accept(_servers[i]._server_fd, (struct sockaddr *) &(client._client_addr), &(client._client_addr_len));
				_fds[_nfds].fd = client._client_fd;
				_fds[_nfds].events = POLLIN;
				_nfds++;
			}
			else
			{
				char	buffer[255];
				int		len;

				setBlocking(client._client_fd, false);
				rc = recv(_fds[i].fd, buffer, sizeof(buffer), 0);
				if (rc <= 0)
				{
					if (rc < 0)	
						perror("recv() failed");
					else
					{
						close(_fds[i].fd);
						std::cout << "Connection fd= " << _fds[i].fd << " closed" << std::endl;
					}
				}
				else
				{
					setBlocking(client._client_fd, true);
					std::cout << buffer << std::endl;
					len = rc;
					rc = send(_fds[i].fd, buffer, len, 0);
					if (rc < 0)
						perror("send() failed");
					setBlocking(client._client_fd, false);
				}
			}
		}
	}
}

void	ServerManager::closeSockets(void){

	for (int i = 0; i < _nfds; i++)
	{
		if (_fds[i].fd >= 0)
			close(_fds[i].fd);
	}
}

void	ServerManager::setBlocking(int fd, bool val){

	int	fl;
	int	res;

	fl = fcntl(fd, F_GETFL, 0);
	if (fl == -1)
		perror("fcntl() failed");
	if (val)
		fl &= ~O_NONBLOCK;
	else
		fl |= O_NONBLOCK;
	res = fcntl(fd, F_SETFL, fl);
	if (fl == -1)
		perror("fcntl() failed");
}
