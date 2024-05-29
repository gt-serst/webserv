/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 09:59:24 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/29 15:51:22 by gt-serst         ###   ########.fr       */
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
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <stdio.h>

#define BUFFER_SIZE 1024

Server::Server(void){}

Server::Server(t_server_scope config) : _config(config){}

Server::~Server(void){

	this->_requests.clear();
}

int	Server::createServerSocket(void){

	int					rc;
	int					flags;
	struct sockaddr_in	server_addr;
	socklen_t			server_addr_len;

	this->_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_fd < 0)
		perror("Socket() failed");

	int reuse = 1;
	rc = setsockopt(this->_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
	if (rc < 0)
		perror("Setsockopt() failed");

	flags = fcntl(this->_fd, F_GETFL, 0);
	if (flags < 0)
		perror("Fcntl() failed");
	fcntl(this->_fd, F_SETFL, flags | O_NONBLOCK);

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	std::cout << this->config.port << std::endl;
	server_addr.sin_port = htons(this->_config.port);

	rc = bind(this->_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if (rc < 0)
		perror("Bind() failed");

	int	connection_backlog = 5;
	rc = listen(this->_fd, connection_backlog);
	if (rc < 0)
		perror("Listen() failed");
	return (this->_fd);
}

int	Server::listenClientConnection(void){

	int					flags;
	int					client_fd;
	struct sockaddr_in	client_addr;
	socklen_t			client_addr_len;

	client_fd = accept(this->_fd, (struct sockaddr *) &(client_addr), (socklen_t *) &(client_addr_len));

	if (client_fd < 0)
		perror("Accept() failed");

	flags = fcntl(client_fd, F_GETFL, 0);
	if (flags < 0)
		perror("Fcntl() failed");
	fcntl(client_fd, F_GETFL, 0);
	return (client_fd);
}

int	Server::readClientSocket(int client_fd){

	int			rc;
	char		buffer[BUFFER_SIZE];
	std::string	stack;

	std::cout << "Start reading" << std::endl;
	while (0 < (rc = recv(client_fd, buffer, sizeof(buffer) - 1, 0)))
	{
		buffer[rc] = '\0';
		stack += buffer;
	}

	if (rc < 0 && errno != EWOULDBLOCK)
	{
		this->closeClientSocket(client_fd);
		perror("Recv failed");
	}
	std::cout << stack << std::endl;
	_requests.insert(std::make_pair(client_fd, stack));
	return (0);
}

void	Server::handleRequest(int client_fd){

	t_locations	location;
	Router		router;
	Response	response;

	Request request = Request(_requests[client_fd], *this);

	if (request.getPathToFile().compare("/favicon.ico") == 0)
		return;
	if (request.getErrorCode() == -1)
	{
		std::string path_to_file = request.getPathToFile();

		location = router.routeRequest(path_to_file, this->_config.locations);

		request.setPathToFile(path_to_file);

		response.handleDirective(request.getPathToFile(), location, this->_config.locations, request, this->_config.error_page_paths);
	}
	else
		response.errorResponse(request.getErrorCode(), request.getErrorMsg(), request.this->_config.error_page_paths);

	_requests.insert(std::make_pair(client_fd, response.getResponse()));
}

int	Server::sendResponse(int client_fd){

	int	rc;
	int	len;

	len = _requests[client_fd].length();
	std::cout << "Response: " << std::endl << response << std::endl;
	std::cout << "Response len: " << len << std::endl;
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

	if (this->_fd > 0)
		close(this->_fd);
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
