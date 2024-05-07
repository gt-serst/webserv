/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:04:51 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/07 16:01:53 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include <vector>

ServerManager::ServerManager(){}

ServerManager::~ServerManager(){}

void	ServerManager::launchServers(void){

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
		_servers[i]._server_addr.sin_port = htons(_servers[i]._port);
		_servers[i]._server_addr_len = sizeof(_servers[i]._server_addr);
		rc = bind(_servers[i]._server_fd, (struct sockaddr *) &(_servers[i]._server_addr), &(_servers[i]._server_addr_len));
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
	}
	_nfds = i;
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
			if (_fds[i].revents == 0)
				continue;
			if (_fds[i].revents != POLLIN)
				break;
			if (_fds[i].fd == _servers[i]._server_fd)
				while (1)
				{
					client._client_fd = accept(_servers[i]._server_fd, (struct sockaddr *) &(client._client_addr), (socklen_t *) &(client._client_addr_len));
					if (client._client_fd < 0)
						break;
					_fds[_nfds].fd = client._client_fd;
					_fds[_nfds].events = POLLIN;
					_nfds++;	
				}
			else
			{
				while (1)
				{
					char	buffer[255];
					int		len;

					rc = recv(_fds[i].fd, buffer, sizeof(buffer), 0);
					if (rc < 0)
					{
						perror("recv() failed");
						break;
					}
					if (rc == 0)
						break;
					len = buffer.c_str().length();
					rc = send(_fds[i].fd, buffer.c_str(), len, 0);
					if (rc < 0)
					{
						perror("send() failed");
						break;
					}
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
