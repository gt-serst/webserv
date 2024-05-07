#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <poll.h>
#include <fcntl.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <cstdlib>

static std::string	handle_get_request(std::string& filename)
{
	std::string	response;
	char*		home;
	
	//std::cout << "|" << filename << "|" << std::endl;
	std::ifstream file(filename);
	if (!file.is_open())
	{
		response = "HTTP/1.1 404 Not Found\r\n\r\n";
		//write(client_fd, response, response.length());
	}
	else
	{
		std::ostringstream content;
    		content << file.rdbuf();
    		file.close();
		response = "HTTP/1.1 200 OK\r\n\r\n" + content.str();
		//write(client_fd, response.c_str(), response.length());
	}
	return (response);
}

static std::string	parse_request(std::string buffer)
{
	int		first_delim;
	int		second_delim;
	std::string	request_methods;
	std::string	path_to_file;
	std::string	response;
	
	if (buffer.empty())
		return (response);
	else
	{
		first_delim = buffer.find(' ');
		second_delim = buffer.find(' ', first_delim + 1);
		request_methods = buffer.substr(0, first_delim);
		path_to_file = buffer.substr(first_delim + 1, second_delim - first_delim - 1);
		//std::cout << "|" << request_methods << "|" << std::endl;
		//std::cout << "|" << path_to_file << "|"  << std::endl;
		if (path_to_file == "/")
			path_to_file += "index.html";
		if (request_methods == std::string("GET"))
		{
			char* home = getenv("HOME");
			std::string filename = std::string(home) + "/Desktop/webserv/var/www/html" + path_to_file;
			response = handle_get_request(filename);
			return (response);
		}
		return (response);
	}
}

int	main(int argc, char ** argv)
{
	int	server_fd;
	int	client_fd;
	int	rc;
	struct pollfd fds[200];
	int    nfds = 1;
	int    timeout;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);

	int reuse = 1;
	rc = setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
	if (rc < 0)
		exit(1);

	fcntl(server_fd, F_SETFL, O_NONBLOCK);

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(4242);
	int	server_addr_len = sizeof(server_addr);
	rc = bind(server_fd, (struct sockaddr *) &server_addr, &server_addr_len);
	if (rc < 0)
		exit(1);

	int	connection_backlog = 5;
	rc = listen(server_fd, connection_backlog);
	if (rc < 0)
		exit(1);

	memset(fds, 0, sizeof(fds));
	fds[0].fd = server_fd;
	fds[0].events = POLLIN;

	timeout = (3 * 60 * 1000);

	while (1)
	{
		rc = poll(fds, nfds, timeout);
		if (rc < 0)
			break;
		if (rc == 0)
			break;
		for (size_t i = 0; i < nfds; i++)
		{
			struct sockaddr_in client_addr;
    		int client_addr_len  = sizeof(struct sockaddr_in);

			if (fds[i].revents == 0)
				continue;
			if (fds[i].revents != POLLIN)
				break;
			if (fds[i].fd == server_fd)
			{
				while (1)
				{
					client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
					if (client_fd < 0)
						break;
					fds[nfds].fd = client_fd;
          			fds[nfds].events = POLLIN;
          			nfds++;
				}
			}
			else
			{
				while (1)
				{
					std::string response;
					char	buffer[255];
					int		len;

					rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
					if (rc < 0)
						break;
					if (rc == 0)
						break;
					response = parse_request(buffer);
					if (response.empty())
						break;
					std::cout << response << std::endl;
					len = response.length();
					rc = send(fds[i].fd, response.c_str(), len, 0);
					if (rc < 0)
						break;
				}
			}
		}
	}
	for (size_t i = 0; i < nfds; i++)
  	{
    	if(fds[i].fd >= 0)
      		close(fds[i].fd);
  	}
	return (0);
}
