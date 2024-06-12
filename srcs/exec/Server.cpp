/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 09:59:24 by gt-serst          #+#    #+#             */
/*   Updated: 2024/06/12 14:23:02 by gt-serst         ###   ########.fr       */
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
	if (rc == 0 || rc == -1)
	{
		if (!rc)
			perror("Nothing more to read");
		else
		{
			this->closeClientSocket(client_fd);
			perror("Recv failed");
		}
		return (-1);
	}
	buffer[rc] = '\0';
	stack += buffer;
	// if (rc == 0)
	// {
	// 	this->closeClientSocket(client_fd);
	// 	perror("Favicon cookies read");
	// 	return (-1);
	// }
	// buffer[rc] = '\0';
	// stack += buffer;
	// while (0 < (rc = recv(client_fd, buffer, sizeof(buffer) - 1, 0)))
	// {
	// 	buffer[rc] = '\0';
	// 	stack += buffer;
	// }
	// if (rc == -1)
	// {
	// 	this->closeClientSocket(client_fd);
	// 	perror("Recv failed");
	// 	return (-1);
	// }
	_requests[client_fd] += stack;
	std::cout << "Printing stack" << std::endl;
	std::cout << _requests[client_fd] << std::endl;
	size_t i = _requests[client_fd].find("\r\n\r\n");
	//std::cout << i << std::endl;
	// read the content header
	if (i != std::string::npos) // si fin des headers trouvé alors vérification si il y a un chunked ou un content length
	{
		if(_requests[client_fd].find("Transfer-Encoding: chunked") != std::string::npos)
		{

			std::cout << "Chunked detected" << std::endl;
			if (_requests[client_fd].find("0\r\n\r\n") != std::string::npos)
			{
				std::cout << "La requête est complète!" << std::endl;
				return (0);
			}
			else
			{
				std::cout << "La requête n'est pas complète!" << std::endl;
				return (1);
			}
		}
		size_t pos = _requests[client_fd].find("Content-Length: ");
		if (pos != std::string::npos)
		{
			std::cout << "Content-Length detected" << std::endl;
			size_t end_of_line = _requests[client_fd].find("\r\n", pos);
			if (end_of_line != std::string::npos)
			{
				// Extraire la valeur de Content-Length
				size_t length_start = pos + 16; // "Content-Length: " est de 16 caractères
				size_t content_length = std::stoi(_requests[client_fd].substr(length_start, end_of_line - length_start));

				// Vérifier si toute la requête a été reçue
				if (_requests[client_fd].size() >= i + content_length + 4)
				{
					std::cout << "La requête est complète!" << std::endl;
					return (0); // La requête complète a été reçue
				}
				else
				{
					std::cout << "La requête n'est pas complète!" << std::endl;
					return (1); // La requête n'est pas encore complète
				}
			}
		}
		std::cout << "Chunk, Content-length et EOF non trouvé" << std::endl;
		return (0); // Content-Length non trouvé ou fin de ligne non trouvée
	}
	// tous les headers ne sont pas présent car \r\n\r\n n'a pas été trouvé
	std::cout << "Je n'ai pas tous les headers" << std::endl;
	return (1);

	//on return 1 tant que pas fini ou si erreur
	//creer un state en parallele pour cette fonction
	//Request request;
	//creer getState
	//if/else ou switch
	//if State == start
	// 	First line parsing
	//else if state == first_line_ok
	//	header parsing
	//else if state == headers_ok
	//	check headers return 0 ou en fonction
	//else if state == parsebody/chunk/multiform
	//	parse body + check de validité et return 1
	// Call to first line parsing function
	// Headers parsing
	// Call to headers parsing function
}

int	Server::handleRequest(int client_fd){

	t_locations	loc;
	Router		router;
	Response	response;

	Request request(_requests[client_fd], *this);
	std::cout << std::endl;
	std::cout << std::endl;
	if (request.getPathToFile().find("/favicon.ico") != std::string::npos)
	{
		std::cout << "Favicon detected" << std::endl;
		return (-1);
	}
	response.setVersion("1.1");
	//response.setVersion(request.getVersion());
	if (request.getErrorCode() == -1)
	{
		std::string path_to_file = request.getPathToFile();

		router.routeRequest(path_to_file, loc, this->_config.locations, response);
		std::cout << "Redir happened: " << response.getRedir() << std::endl;
		if (response.getRedir() == true)
			response.generateResponse();
		else
		{
			request.setPathToFile(path_to_file);

			std::cout << "Path to file: " << request.getPathToFile() << std::endl;

			response.handleDirective(request.getPathToFile(), loc, request, *this);
		}
	}
	else
	{
		std::cout << request.getErrorCode() << std::endl;
		response.errorResponse(request.getErrorCode(), request.getErrorMsg(), getConfig().error_page_paths);
	}
	_requests.erase(client_fd);
	_requests.insert(std::make_pair(client_fd, response.getResponse()));

	std::cout << std::endl;
	std::cout << std::endl;
	return (0);
}

int	Server::sendResponse(int client_fd){

	int	rc;
	int	len;

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "Response" << std::endl;
	std::cout << _requests[client_fd] << std::endl;
	len = _requests[client_fd].length();
	rc = send(client_fd, _requests[client_fd].c_str(), len, 0);
	std::cout << "/////////////////////// MMMMH J'AI MANGÉ UNE REQUÊTE ////////////////////////" << std::endl;
	// if (rc == -1)
	// {
	// 	this->closeClientSocket(client_fd);
	// 	perror("Send() failed");
	// 	return (-1);
	// }
	if (rc == 0 || rc == -1)
	{
		if (!rc)
		{
			_requests.erase(client_fd);
			perror("Nothing more to send");
			return (0);
		}
		else
		{
			this->closeClientSocket(client_fd);
			perror("Send() failed");
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
