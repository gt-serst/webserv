/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:04:51 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/14 10:03:29 by gt-serst         ###   ########.fr       */
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

#define MAX_FD 20
#define BUFFER_SIZE 9999

ServerManager::ServerManager(){}

ServerManager::~ServerManager(){}

void	ServerManager::launchServers(void){

	Server server(8080);
	_servers.push_back(server);
	createSockets();
	checkSockets();
	closeSockets();
}

void	ServerManager::createSockets(void){

	int		rc;
	int		flags;

	memset(_pollfds, 0, sizeof(_pollfds));
	
	for (int x = 0; x < 1; x++)
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
		int port = 8080;
		_servers[x]._server_addr.sin_port = htons(port);

		rc = bind(_servers[x]._server_fd, (struct sockaddr *) &_servers[x]._server_addr, sizeof(_servers[x]._server_addr));
		if (rc < 0)
			perror("Bind() failed");

		int	connection_backlog = 5;
		rc = listen(_servers[x]._server_fd, connection_backlog);
		if (rc < 0)
			perror("Listen() failed");

		_pollfds[x].fd = _servers[x]._server_fd;
		_pollfds[x].events = POLLIN;
		nfds++;
	}
}

void	ServerManager::checkSockets(void){

	int	rc;
	int	flags;
	int	nready;

	while (1)
	{
		std::cout << "New loop" << std::endl;

		rc = poll(_pollfds, nfds, -1);
		if (rc < 0)
			perror("Poll() failed");

		for (int x = 0; x < _servers.size(); x++)
		{
			std::cout << "Listening loop" << std::endl;
			std::cout << "Server number " << x << std::endl;
			if (_pollfds[x].revents & POLLIN)
			{
				std::cout << _pollfds[x].fd << std::endl;
				std::cout << _servers[x]._server_fd << std::endl;
				Client client;
				std::cout << "Waiting for connection" << std::endl;

				client._client_fd = accept(_pollfds[x].fd, (struct sockaddr *) &(client._client_addr), (socklen_t *) &(client._client_addr_len));
				if (client._client_fd < 0)
					perror("Accept() failed");

				std::cout << "Connected" << std::endl;

				flags = fcntl(client._client_fd, F_GETFL, 0);
				if (flags < 0)
					perror("Fcntl() failed");
				fcntl(client._client_fd, F_SETFL, flags | O_NONBLOCK);

				for (int z = _servers.size(); z < MAX_FD; z++)
				{
					std::cout << _pollfds[z].fd << std::endl;
					std::cout << "Poll elem initialization loop" << std::endl;
					if (_pollfds[z].fd == 0)
					{
						std::cout << "Elem initialization" << std::endl;
						_pollfds[z].fd = client._client_fd;
						_pollfds[z].events = POLLIN;
						nfds++;
						break;
					}
				}
				for (int y = _servers.size(); y < nfds; y++)
				{
					//std::cout << "Writing loop" << std::endl;
					if (_pollfds[y].revents & POLLIN)
					{
						char	buffer[BUFFER_SIZE];
						int		len;
						std::string response;

						//std::cout << "Waiting to write" << std::endl;
						rc = recv(_pollfds[y].fd, buffer, sizeof(buffer) - 1, 0);
						if (rc < 0)
							perror("Recv() failed");
						else
						{
							buffer[rc] = '\0';
							std::cout << buffer << std::endl;

							std::string response = "HTTP/1.1 200 OK\r\n";
							std::string body = "<!DOCTYPE html>\n"
                			"<html>\n"
                 			"<head>\n"
                 			"<title>Welcome to nginx!</title>\n"
                 			"<style>\n"
                 			"html { color-scheme: light dark; }\n"
                 			"body { width: 35em; margin: 0 auto;\n"
                 			"font-family: Tahoma, Verdana, Arial, sans-serif; }\n"
                 			"</style>\n"
                 			"</head>\n"
                 			"<body>\n"
                 			"<h1>Welcome to nginx!</h1>\n"
                 			"<p>If you see this page, the nginx web server is successfully installed and\n"
                 			"working. Further configuration is required.</p>\n"
                 			"<p>For online documentation and support please refer to\n"
                 			"<a href=\"http://nginx.org/\">nginx.org</a>.<br/>\n"
                 			"Commercial support is available at\n"
                 			"<a href=\"http://nginx.com/\">nginx.com</a>.</p>\n"
                 			"<p><em>Thank you for using nginx.</em></p>\n"
                 			"</body>\n"
                 			"</html>\n";

							response = std::string("HTTP/1.1 200 OK") + std::string("Content-Type: application/octet-stream\r\nContent-Length: ") + std::to_string(body.length()) + std::string("\r\n\r\n") + body + std::string("\r\n\r\n");

							len = response.length();
							rc = send(_pollfds[y].fd, response.c_str(), len, 0);
							if (rc < 0)
								perror("Send() failed");
						}
						close(_pollfds[y].fd);
						_pollfds[y].fd = 0;
						_pollfds[y].events = 0;
						_pollfds[y].revents = 0;
						nfds--;
					}
					if (--nready <= -1)
						break;
				}
			}
		}

		//std::cout << "End loop" << std::endl;

	}	
}

void	ServerManager::closeSockets(void){

	for (int x = 0; x < MAX_FD; x++)
	{
		if (_pollfds[x].fd >= 0)
			close(_pollfds[x].fd);
		close(_pollfds[x].fd);
	}
}
