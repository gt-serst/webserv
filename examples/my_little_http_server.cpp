#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

#define MAX_CL 20
#define MAX_SV 20
#define BUFFER_SIZE 9999

int	main(int argc, char** argv)
{
	int		server_fd[MAX_SV];
	int		client_fd;
	int		rc;
	int		flags;

	struct pollfd	pollfd_clients[MAX_CL];
	struct pollfd	pollfd_servers[MAX_SV];
	memset(pollfd_clients, 0, sizeof(pollfd_clients));
	memset(pollfd_servers, 0, sizeof(pollfd_servers));

	for (int x = 0; x < MAX_SV; x++)
	{
		server_fd[x] = socket(AF_INET, SOCK_STREAM, 0);
		if (server_fd[x] < 0)
			perror("Socket() failed");

		int reuse = 1;
		rc = setsockopt(server_fd[x], SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
		if (rc < 0)
			perror("Setsockopt() failed");
	
		flags = fcntl(server_fd[x], F_GETFL, 0);
		if (flags < 0)
			perror("Fcntl() failed");
		fcntl(server_fd[x], F_SETFL, flags | O_NONBLOCK);

		struct sockaddr_in server_addr;
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = INADDR_ANY;
		int port = 8080 - x;
		server_addr.sin_port = htons(port);

		rc = bind(server_fd[x], (struct sockaddr *) &server_addr, sizeof(server_addr));
		if (rc < 0)
			perror("Bind() failed");

		int	connection_backlog = 50;
		rc = listen(server_fd[x], connection_backlog);
		if (rc < 0)
			perror("Listen() failed");

		pollfd_servers[x].fd = server_fd[x];
		pollfd_servers[x].events = POLLIN;
	}

	int nready;
	struct sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);
	
	while (1)
	{
		std::cout << "New loop" << std::endl;

		rc = poll(pollfd_servers, MAX_SV, -1);
		if (rc < 0)
			perror("Poll() failed");

		for (int x = 0; x < MAX_SV; x++)
		{
			std::cout << "Listening loop" << std::endl;
			
			if (pollfd_servers[x].revents & POLLIN)
			{
				std::cout << "Waiting for connection" << std::endl;
				
				client_fd = accept(pollfd_servers[x].fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
				if (client_fd < 0)
					perror("Accept() failed");
				
				std::cout << "Connected" << std::endl;

				flags = fcntl(client_fd, F_GETFL, 0);
				if (flags < 0)
					perror("Fcntl() failed");
				fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

				for (int z = 0; z < MAX_CL; z++) 
				{
					std::cout << "Poll elem initialization loop" << std::endl;
					if (pollfd_clients[z].fd == 0)
					{
						std::cout << "Elem initialization" << std::endl;
						pollfd_clients[z].fd = client_fd;
						pollfd_clients[z].events = POLLIN;
						nready = poll(pollfd_clients, MAX_CL, -1);
						if (nready < 0)
							perror("Poll() failed");	
						break;
					}
				}
				for (int y = 0; y < MAX_CL; y++)
				{
					std::cout << "Writing loop" << std::endl;
					if (pollfd_clients[y].revents & POLLIN)
					{
						char	buffer[BUFFER_SIZE];
						int		len;
						std::string response;

						std::cout << "Waiting to write" << std::endl;
						rc = recv(pollfd_clients[y].fd, buffer, sizeof(buffer) - 1, 0);
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
							rc = send(pollfd_clients[y].fd, response.c_str(), len, 0);
							if (rc < 0)
								perror("Send() failed");
						}
						close(pollfd_clients[y].fd);
						pollfd_clients[y].fd = 0;
						pollfd_clients[y].events = 0;
						pollfd_clients[y].revents = 0;
					}
					if (--nready <= -1)
						break;
				}
			}
		}

		std::cout << "End loop" << std::endl;
	
	}

	for (int x = 0; x < 2; x++)
		close(pollfd_servers[x].fd);
	return (0);
}
