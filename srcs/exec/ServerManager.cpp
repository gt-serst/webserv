/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:04:51 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/16 18:25:20 by gt-serst         ###   ########.fr       */
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
#include <sys/select.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

#define BUFFER_SIZE 9999

ServerManager::ServerManager(){}

ServerManager::~ServerManager(){}

void	ServerManager::launchServers(void){

	Server alpha(8080);
	Server bravo(9090);
	_servers.push_back(alpha);
	_servers.push_back(bravo);
	createSockets();
	checkSockets();
	closeServerSockets();
}

void	ServerManager::createSockets(void){

	int		rc;
	int		flags;

	for (int x = 0; x < 2; x++)
	{
		_servers[x]._server_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (_servers[x]._server_fd < 0)
			perror("Socket() failed");

		int reuse = 1;
		rc = setsockopt(_servers[x]._server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
		if (rc < 0)
			perror("Setsockopt() failed");

		flags = fcntl(_servers[x]._server_fd, F_GETFL, 0);
		if (flags < 0)
			perror("Fcntl() failed");
		fcntl(_servers[x]._server_fd, F_SETFL, flags | O_NONBLOCK);

		_servers[x]._server_addr.sin_family = AF_INET;
		_servers[x]._server_addr.sin_addr.s_addr = INADDR_ANY;
		_servers[x]._server_addr.sin_port = htons(_servers[x].getPort());

		rc = bind(_servers[x]._server_fd, (struct sockaddr *) &_servers[x]._server_addr, sizeof(_servers[x]._server_addr));
		if (rc < 0)
			perror("Bind() failed");

		int	connection_backlog = 5;
		rc = listen(_servers[x]._server_fd, connection_backlog);
		if (rc < 0)
			perror("Listen() failed");
	}
}

void	ServerManager::checkSockets(void){

	int		rc;
	fd_set	current_sockets;
	fd_set	ready_sockets;

	FD_ZERO(&current_sockets);
	for (int x = 0; x < _servers.size(); x++)
		FD_SET(_servers[x]._server_fd, &current_sockets);

	while (1)
	{
		//std::cout << "New loop" << std::endl;
		
		ready_sockets = current_sockets;
		
		rc = select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL);
		if (rc < 0)
			perror("Select() failed");

		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &ready_sockets))
			{
				if (socketIsServer(i))
					listenConnections(i);	
				else
				{
					if (readClientSocket(i) < 0)
						perror("Recv failed");
					else
						handleRequest(i);
				}
			}
		}

		//std::cout << "End loop" << std::endl;
	}
}

void	ServerManager::listenConnections(unsigned int fd){

	int		flags;
	Client client;

	client._client_fd = accept(fd, (struct sockaddr *) &(client._client_addr), (socklen_t *) &(client._client_addr_len));

	Client copy_client(client);
	_clients.push_back(copy_client);

	if (client._client_fd < 0)
		perror("Accept() failed");
				
	flags = fcntl(client._client_fd, F_GETFL, 0);
	if (flags < 0)
		perror("Fcntl() failed");	
	fcntl(client._client_fd, F_GETFL, 0);

	FD_SET(client._client_fd, &current_sockets);
}

int	ServerManager::readClientSocket(unsigned int fd){

	int		rc;
	char	buffer[BUFFER_SIZE];

	rc = recv(fd, buffer, sizeof(buffer) - 1, 0);
	buffer[rc] = '\0';
	return (rc);
}

void	ServerManager::handleRequest(unsigned int fd, char* buffer){

	sendResponse(fd, buffer);
}

void	ServerManager::sendResponse(unsigned int fd, char* buffer){

	int	rc;
	int	len;

	len = buffer.length();
	rc = send(fd, buffer, len, 0);
	if (rc < 0)
		perror("Send() failed");
	FD_CLR(fd, &current_sockets);
	close(fd);
}

bool	ServerManager::socketIsServer(unsigned int fd) const{
	
	for (int i = 0; i < _servers.size(); i++)
	{
		if (socket_fd == _servers[i]._server_fd)
			return (true);
	}
	return (false);
}

void	ServerManager::closeServerSockets(void) const{

	for (int x = 0; x < _servers.size(); x++)
		close(_servers[x]._server_fd);
}
