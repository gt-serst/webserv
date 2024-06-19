/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 09:59:24 by gt-serst          #+#    #+#             */
/*   Updated: 2024/06/19 13:54:34 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerManager.hpp"
#include "../parser/confParser.hpp"
#include "../request/Request.hpp"
#include "../response/Router.hpp"
#include "../response/Response.hpp"
#include "../webserv.hpp"
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
		std::cerr << "Socket() failed" << std::endl;

	int reuse = 1;
	rc = setsockopt(this->_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
	if (rc < 0)
	{
		std::cerr << "Setsockopt() failed" << std::endl;
		return (-1);
	}
	flags = fcntl(this->_fd, F_GETFL, 0);
	if (flags < 0)
	{
		std::cerr << "Fcntl() failed" << std::endl;
		return (-1);
	}
	fcntl(this->_fd, F_SETFL, flags | O_NONBLOCK);

	std::memset((char *)&_server_addr, 0, sizeof(_server_addr));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = INADDR_ANY;
	_server_addr.sin_port = htons(this->_config.port);

	rc = bind(this->_fd, (struct sockaddr *) &(_server_addr), sizeof(_server_addr));
	if (rc < 0)
	{
		std::cerr << "Bind() failed" << std::endl;
		return (-1);
	}
	int	connection_backlog = 1000;
	rc = listen(this->_fd, connection_backlog);
	if (rc < 0)
	{
		std::cerr << "Listen() failed" << std::endl;
		return (-1);
	}
	return (this->_fd);
}

int	Server::listenClientConnection(void){

	int					flags;
	int					client_fd;
	socklen_t			client_addr_len = sizeof(_client_addr);

	client_fd = accept(this->_fd, (struct sockaddr *) &(_client_addr), &client_addr_len);

	if (client_fd < 0)
	{
		std::cerr << "Accept() failed" << std::endl;
		return (-1);
	}
	flags = fcntl(client_fd, F_GETFL, 0);
	if (flags < 0)
	{
		std::cerr << "Fcntl() failed" << std::endl;
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
	if (rc == 0 || rc == -1)
	{
		if (!rc)
		{
			std::cout << "Client close connection" << std::endl;
			return (0);
		}
		else
		{
			this->closeClientSocket(client_fd);
			std::cerr << "Recv() failed" << std::endl;
			return (-1);
		}
	}
	buffer[rc] = '\0';
	stack += buffer;
	_requests[client_fd] += stack;
	//std::cout << _requests[client_fd] << std::endl;
	// Vérification pour des requêtes envoyées en plusieurs parties
	size_t i = _requests[client_fd].find("\r\n\r\n");
	if (i != std::string::npos) // Si fin des headers trouvé alors vérification si il y a un chunked ou un content length
	{
		if(_requests[client_fd].find("Transfer-Encoding: chunked") != std::string::npos)
		{

			if (_requests[client_fd].find("0\r\n\r\n") != std::string::npos)
			{
				std::cout << "La requête complète a été reçue" << std::endl;
				return (0); // La requête complète a été reçue
			}
			else
			{
				std::cout << "La requête n'est pas encore complète" << std::endl;
				return (1); // La requête n'est pas encore complète
			}
		}
		size_t pos = _requests[client_fd].find("Content-Length: ");
		if (pos != std::string::npos)
		{
			size_t end_of_line = _requests[client_fd].find("\r\n", pos);
			if (end_of_line != std::string::npos)
			{
				// Extraire la valeur de Content-Length
				size_t length_start = pos + 16; // "Content-Length: " est de 16 caractères
				size_t content_length = ft_atoi(_requests[client_fd].substr(length_start, end_of_line - length_start).c_str());

				// Vérifier si toute la requête a été reçue
				if (_requests[client_fd].size() >= i + content_length + 4)
				{
					std::cout << "La requête complète a été reçue" << std::endl;
					return (0); // La requête complète a été reçue
				}
				else
				{
					std::cout << "La requête n'est pas encore complète" << std::endl;
					return (1); // La requête n'est pas encore complète
				}
			}
		}
		std::cout << "Content-Length non trouvé ou fin de ligne non trouvée" << std::endl;
		return (0); // Content-Length non trouvé ou fin de ligne non trouvée
	}
	// Tous les headers ne sont pas présent car \r\n\r\n n'a pas été trouvé
	std::cout << "Tous les headers ne sont pas présent" << std::endl;
	return (1);
}

int	Server::handleRequest(int client_fd){

	t_locations	loc;
	Router		router;
	Response	response;

	Request request(_requests[client_fd], *this);
	response.setVersion(request.getVersion());
	if (request.getPathToFile().find("/favicon.ico") != std::string::npos)
		response.errorResponse(404, "Not Found", getConfig().error_page_paths);
	else if (request.getErrorCode() == -1)
	{
		std::string path_to_file = request.getPathToFile();

		router.routeRequest(path_to_file, loc, this->_config.locations, response);
		// If redir, generate a redir response and the browser will resend a GET request with the new direction
		if (response.getRedir() == true)
			response.generateResponse();
		else
		{
			request.setPathToFile(path_to_file);

			response.handleDirective(request.getPathToFile(), loc, request, *this);
			if (response.getDefaultFile() == true)
			{
				// When a default file is found we relaunch all the routine, router and response process
				response.setDefaultFile(false);
				response.handleDirective(request.getPathToFile(), loc, request, *this);
			}
			else if (response.getCGI() == true)
			{
				// If CGI is runned in the response process, we do not have to send a response in the client socket because script already done it
				_requests.erase(client_fd);
				return (1);
			}
		}
	}
	else
		// Catch error from the request parser
		response.errorResponse(request.getErrorCode(), request.getErrorMsg(), getConfig().error_page_paths);

	_requests.erase(client_fd);
	_requests.insert(std::make_pair(client_fd, response.getResponse()));

	return (0);
}

int	Server::sendResponse(int client_fd){

	int	rc;
	int	len;

	len = _requests[client_fd].length();
	rc = send(client_fd, _requests[client_fd].c_str(), len, 0);
	if (rc == 0 || rc == -1)
	{
		if (!rc)
		{
			_requests.erase(client_fd);
			std::cout << "Client close connection" << std::endl;
			return (0);
		}
		else
		{
			this->closeClientSocket(client_fd);
			std::cerr << "Send() failed" << std::endl;
			return (-1);
		}
	}
	else
	{
		_requests.erase(client_fd);
		return (0);
	}
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

int	Server::getClientFd(void) const
{
	return (this->_requests.begin()->first);
}

t_server_scope	Server::getConfig() const{

	return _config;
}

struct sockaddr_in	Server::getClientAddr() const{

	return _client_addr;
}

std::map<int, std::string>	Server::getRequests(void) const{

	return _requests;
}

void	Server::setRequests(std::map<int, std::string> requests){

	_requests = requests;
}
