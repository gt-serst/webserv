/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:04:51 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/28 18:25:08 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "../parser/confParser.hpp"
#include "../request/Request.hpp"
#include "../response/Router.hpp"
#include <vector>
#include <algorithm>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <cerrno>

#define BUFFER_SIZE 1024

ServerManager::ServerManager(){}

ServerManager::~ServerManager(){}

void	ServerManager::launchServer(t_server_scope *servers, int nb_servers){

	int	i;

	i = 0;
	while (i <= nb_servers)
	{
		_servers.push_back(Server(servers[i]));
		i++;
	}
	createServerSocket();
	serverRoutine();
	closeServerSocket();
}

void	ServerManager::createServerSocket(void){

	int		rc;
	int		flags;

	std::cout << _servers.size() << std::endl;
	for (size_t x = 0; x < _servers.size(); x++)
	{
		_servers[x].server_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (_servers[x].server_fd < 0)
			perror("Socket() failed");

		int reuse = 1;
		rc = setsockopt(_servers[x].server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
		if (rc < 0)
			perror("Setsockopt() failed");

		flags = fcntl(_servers[x].server_fd, F_GETFL, 0);
		if (flags < 0)
			perror("Fcntl() failed");
		fcntl(_servers[x].server_fd, F_SETFL, flags | O_NONBLOCK);

		_servers[x].server_addr.sin_family = AF_INET;
		_servers[x].server_addr.sin_addr.s_addr = INADDR_ANY;
		std::cout << _servers[x].getConfig().port << std::endl;
		_servers[x].server_addr.sin_port = htons(_servers[x].getConfig().port);

		rc = bind(_servers[x].server_fd, (struct sockaddr *) &_servers[x].server_addr, sizeof(_servers[x].server_addr));
		if (rc < 0)
			perror("Bind() failed");

		int	connection_backlog = 5;
		rc = listen(_servers[x].server_fd, connection_backlog);
		if (rc < 0)
			perror("Listen() failed");
	}
}

void	ServerManager::serverRoutine(void){

	int		rc;

	FD_ZERO(&_current_sockets);
	for (int x = 0; x < static_cast<int>(_servers.size()); x++)
		FD_SET(_servers[x].server_fd, &_current_sockets);

	while (1)
	{
		std::cout << "New loop" << std::endl;

		_ready_sockets = _current_sockets;

		rc = select(FD_SETSIZE, &_ready_sockets, NULL, NULL, NULL);
		if (rc < 0)
			perror("Select() failed");

		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &_ready_sockets))
			{
				if (serverEvent(i) == 1)
					listenClientConnection(i);
				else
				{
					std::string	data;

					data = readClientSocket(i);
					handleRequest(i, data);
				}
			}
		}

		std::cout << "End loop" << std::endl;
	}
}

void	ServerManager::listenClientConnection(unsigned int fd){

	int	flags;

	_current_client.client_fd = accept(fd, (struct sockaddr *) &(_current_client.client_addr), (socklen_t *) &(_current_client.client_addr_len));

	if (_current_client.client_fd < 0)
		perror("Accept() failed");

	flags = fcntl(_current_client.client_fd, F_GETFL, 0);
	if (flags < 0)
		perror("Fcntl() failed");
	fcntl(_current_client.client_fd, F_GETFL, 0);

	FD_SET(_current_client.client_fd, &_current_sockets);
}

std::string	ServerManager::readClientSocket(unsigned int fd){

	int			rc;
	char		buffer[BUFFER_SIZE];
	std::string	data;

	std::cout << "Start reading" << std::endl;
	while (0 < (rc = recv(fd, buffer, sizeof(buffer) - 1, 0)))
	{
		buffer[rc] = '\0';
		data += buffer;
	}

	if (rc < 0 && errno != EWOULDBLOCK)
		perror("Recv failed");

	std::cout << data << std::endl;
	return (data);
}

void	ServerManager::handleRequest(unsigned int fd, std::string data){

	t_locations	location;

	_current_request = Request(data, _current_server);

	if (_current_request.getPathToFile().compare("/favicon.ico") == 0)
		return;
	if (_current_request.getErrorCode() == -1)
	{
		std::string path_to_file = _current_request.getPathToFile();

		location = _router.routeRequest(path_to_file, _current_server.getConfig().locations);

		_current_request.setPathToFile(path_to_file);

		_current_response.handleDirective(_current_request.getPathToFile(), location, _current_server.getConfig().locations, _current_request, _current_server.getConfig().error_page_paths);
	}
	else
		_current_response.errorResponse(_current_request.getErrorCode(), _current_request.getErrorMsg(), _current_request._server.getConfig().error_page_paths);

	sendResponse(fd, _current_response.getResponse());
}

void	ServerManager::sendResponse(unsigned int fd, std::string response){

	int	rc;
	int	len;

//	len = buffer.length();
//	std::string response = "HTTP/1.1 200 OK\r\n";
//	std::string body = "<!DOCTYPE html>\n"
//    "<html>\n"
//    "<head>\n"
//    "<title>Welcome to nginx!</title>\n"
//    "<style>\n"
//    "html { color-scheme: light dark; }\n"
//    "body { width: 35em; margin: 0 auto;\n"
//    "font-family: Tahoma, Verdana, Arial, sans-serif; }\n"
//    "</style>\n"
//    "</head>\n"
//    "<body>\n"
//    "<h1>Welcome to nginx!</h1>\n"
//    "<p>If you see this page, the nginx web server is successfully installed and\n"
//    "working. Further configuration is required.</p>\n"
//    "<p>For online documentation and support please refer to\n"
//    "<a href=\"http://nginx.org/\">nginx.org</a>.<br/>\n"
//    "Commercial support is available at\n"
//    "<a href=\"http://nginx.com/\">nginx.com</a>.</p>\n"
//    "<p><em>Thank you for using nginx.</em></p>\n"
//    "</body>\n"
//    "</html>\n";
//
//     response = std::string("HTTP/1.1 200 OK") + std::string("Content-Type: application/octet-stream\r\nContent-Length: ") + std::to_string(body.length()) + std::string("\r\n\r\n") + body + std::string("\r\n\r\n");

//    len = response.length();
	len = response.length();
	std::cout << "Response: " << std::endl << response << std::endl;
	std::cout << "Response len: " << len << std::endl;
	rc = send(fd, response.c_str(), len, 0);
	if (rc < 0)
		perror("Send() failed");
	FD_CLR(fd, &_current_sockets);
	close(fd);
}

bool	ServerManager::serverEvent(unsigned int fd){

	for (int i = 0; i < static_cast<int>(_servers.size()); i++)
	{
		if (static_cast<int>(fd) == _servers[i].server_fd)
		{
			_current_server = _servers[i];
			return (true);
		}
	}
	return (false);
}

void	ServerManager::closeServerSocket(void) const{

	for (int x = 0; x < static_cast<int>(_servers.size()); x++)
		close(_servers[x].server_fd);
}
