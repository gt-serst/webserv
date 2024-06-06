/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 09:59:24 by gt-serst          #+#    #+#             */
/*   Updated: 2024/06/06 17:06:39 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerManager.hpp"
#include "../parser/confParser.hpp"
#include "../request/Request.hpp"
#include "../response/Router.hpp"
#include "../response/Response.hpp"
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <stdio.h>

#define BUFFER_SIZE 1024

Server::Server(void){}

Server::Server(t_server_scope config) : _config(config){

	//std::cout << "Server created" << std::endl;
}

Server::~Server(void){

	//std::cout << "Server destroyed" << std::endl;
	_requests.clear();
}

int	Server::createServerSocket(void){

	int					rc;
	int					flags;

	this->_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_fd < 0)
		perror("Socket() failed");

	int reuse = 1;
	rc = setsockopt(this->_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
	if (rc < 0)
	{
		perror("Setsockopt() failed");
		return (-1);
	}
	flags = fcntl(this->_fd, F_GETFL, 0);
	if (flags < 0)
	{
		perror("Fcntl() failed");
		return (-1);
	}
	fcntl(this->_fd, F_SETFL, flags | O_NONBLOCK);

	std::memset((char *)&_server_addr, 0, sizeof(_server_addr));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = INADDR_ANY;
	// std::cout << this->_config.port << std::endl;
	_server_addr.sin_port = htons(this->_config.port);

	rc = bind(this->_fd, (struct sockaddr *) &(_server_addr), sizeof(_server_addr));
	if (rc < 0)
	{
		perror("Bind() failed");
		return (-1);
	}
	int	connection_backlog = 1000;
	rc = listen(this->_fd, connection_backlog);
	if (rc < 0)
	{
		perror("Listen() failed");
		return (-1);
	}
	return (this->_fd);
}

int	Server::listenClientConnection(void){

	int					flags;
	int					client_fd;
	struct sockaddr_in	client_addr;
	socklen_t			client_addr_len;

	client_fd = accept(this->_fd, (struct sockaddr *) &(client_addr), (socklen_t *) &(client_addr_len));

	if (client_fd < 0)
	{
		perror("Accept() failed");
		return (-1);
	}
	flags = fcntl(client_fd, F_GETFL, 0);
	if (flags < 0)
	{
		perror("Fcntl() failed");
		return (-1);
	}
	fcntl(client_fd, F_GETFL, 0);
	return (client_fd);
}

int	Server::readClientSocket(int client_fd){

	int			rc;
	char		buffer[BUFFER_SIZE];
	std::string	stack;

	rc = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	if (rc == 0)
	{
		this->closeClientSocket(client_fd);
		perror("Favicon cookies read");
		return (-1);
	}
	buffer[rc] = '\0';
	stack += buffer;
	while (0 < (rc = recv(client_fd, buffer, sizeof(buffer) - 1, 0)))
	{
		buffer[rc] = '\0';
		stack += buffer;
	}
	if (rc < 0 && errno != EWOULDBLOCK)
	{
		this->closeClientSocket(client_fd);
		perror("Recv failed");
		return (-1);
	}
	//std::cout << stack << std::endl;
	_requests[client_fd] = stack;
	// _requests.insert(std::make_pair(client_fd, stack));
	// rc = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	// if (rc == 0 || rc == -1)
	// {
	// 	this->closeClientSocket(client_fd);
	// 	perror("Recv failed");
	// 	return (-1);
	// }
	// _requests.insert(std::make_pair(client_fd, std::string(buffer)));
	return (0);
}

int	Server::handleRequest(int client_fd){

	t_locations	loc;
	Router		router;
	Response	response;

	//std::cout << "Request send to Request constructor: |" << _requests[client_fd] << "|" << std::endl;
	Request request(_requests[client_fd], *this);

	if (request.getPathToFile().find("/favicon.ico") != std::string::npos)
	{
		// request parsing detect error 405 for favicon
		std::cout << "Favicon detected" << std::endl;
		return (-1);
	}
	response.setVersion(request.getVersion());
	if (request.getErrorCode() == -1)
	{
		if (response.checkPortAndServerName(getConfig()) == false)
			return (-1);

		std::string path_to_file = request.getPathToFile();

		if (router.routeRequest(path_to_file, loc, this->_config.locations) == true)
		{
			request.setPathToFile(path_to_file);

			std::cout << request.getPathToFile() << std::endl;

			response.handleDirective(request.getPathToFile(), loc, request, *this);
		}
		else
			response.errorResponse(301, "The page isn't redirecting properly", getConfig().error_page_paths);
	}
	else
	{
		//std::cout << "Error in parsing" << std::endl;
		response.errorResponse(request.getErrorCode(), request.getErrorMsg(), getConfig().error_page_paths);
	}

	//_requests.insert(std::make_pair(client_fd, response.getResponse()));
	_requests.erase(client_fd);
	_requests.insert(std::make_pair(client_fd, response.getResponse()));
	// _requests[client_fd] = response.getResponse();
	return (0);
}

int	Server::sendResponse(int client_fd){

	int	rc;
	int	len;

	len = _requests[client_fd].length();
	std::cout << _requests[client_fd] << std::endl;
	rc = send(client_fd, _requests[client_fd].c_str(), len, 0);
	if (rc == -1)
	{
		this->closeClientSocket(client_fd);
		perror("Send() failed");
		return (-1);
	}
	else
		return (0);
}

void	Server::closeServerSocket(void){

	if (_fd > 0)
		close(_fd);
	_fd = -1;
}

void	Server::closeClientSocket(int client_fd){

	if (client_fd > 0)
		close(client_fd);
	_requests.erase(client_fd);
}

int	Server::getFd(void) const{

	return _fd;
}

t_server_scope	Server::getConfig() const{

	return _config;
}
