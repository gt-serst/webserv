/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:04:51 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/07 13:21:35 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include <vector>

ServerManager::ServerManager(){}

ServerManager::~ServerManager(){}

void	ServerManager::launchServers(void){

	createSockets();
	checkSocket();
}

void	ServerManager::createSockets(void){

	int	i;
	int	rc;

	i = 0;
	for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		it._server_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (it._server_fd < 0)
		{
			perror("socket() failed");
			exit(-1);
		}
		int reuse = 1;
		rc = setsockopt(it._server_fd, SOL_SOKCET, SO_REUSEPORT, &reuse, sizeof(reuse));
		if (rc < 0)
		{
			perror("setsockopt() failed");
			exit(-1);
		}
		rc = fcntl(it._server_fd, F_SETFL, O_NONBLOCK);
		if (rc < 0)
		{
			perror("fcntl() failed");
			exit(-1);
		}
		it._server_addr.sin_family = AF_INET;
		it._server_addr.sin_addr.s_addr = INADDR_ANY;
		it._server_addr.sin_port = htons(it._port);
		it._server_addr_len = sizeof(it._server_addr);
		rc = bind(it._server_fd, (struct sockaddr *)&(it._server_addr) &(it._server_addr_len));
		if (rc < 0)
		{
			perror("bind() failed");
			exit(-1);
		}
		int connection_backlog = 5;
		rc = listen(it._server_fd, connection_backlog);
		if (rc < 0)
		{
			perror("listen() failed");
			exit(-1);
		}
		memset(_fds, 0, sizeof(_fds));
		_fds[i].fd = it._server_fd;
		_fds[i].events = POLLIN;
		i++;
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
			Client	client;
			client._client_addr_len = sizeof(client._client_addr);
			if (_fds[i].revents == 0)
				continue;
			if (_fds[i].revents != POLLIN)
				break;
			if (_fds[i].fd == )
		}
	}
}
