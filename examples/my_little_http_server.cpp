#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

#define MAX_FD 20

int	main(int argc, char ** argv)
{
	int		server_fd[2];
	int		client_fd;
	int		rc;
	int		flags;

	struct pollfd	pollfds[MAX_FD];
	memset(pollfds, 0, sizeof(pollfds));

	for (int x = 0; x < 2; x++)
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

		int	connection_backlog = 5;
		rc = listen(server_fd[x], connection_backlog);
		if (rc < 0)
			perror("Listen() failed");

		pollfds[x].fd = server_fd[x];
		pollfds[x].events = POLLIN;
	}

	struct sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);

	std::cout << server_fd[0] << std::endl;
	std::cout << server_fd[1] << std::endl;

	while (1)
	{
		rc = poll(pollfds, MAX_FD, -1);
		if (rc < 0)
			perror("Poll() failed");

		for (int x = 0; x < 2; x++)
		{
			for (int y = 0; y < MAX_FD; y++)
			{

				/** Condition pour s'assurer que l'event qui a eu lieu survient bien sur le serveur ou la boucle se situe actuellement  **/
				if (!(pollfds[y].revents & POLLIN))
					continue;
				if (pollfds[y].fd == server_fd[x])
				{
					std::cout << "New connection" << std::endl;
					client_fd = accept(server_fd[x], (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);

					/** Condition qui rejette tous les serveurs qui n'ont pas recu de connexions **/	
					if (client_fd < 0)
					{
						std::cout << "Connection rejected" << std::endl;
						continue;
					}
					else if (client_fd < 0 && x == 1)
						perror("Accept() failed");

					flags = fcntl(client_fd, F_GETFL, 0);
					if (flags < 0)
						perror("Fcntl() failed");
					fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

					for (int z = 1; z < MAX_FD; z++) 
					{	
						if (pollfds[z].fd == 0)
						{
							pollfds[z].fd = client_fd;
							pollfds[z].events = POLLIN;
							break;
						}
					}
				}

				/** Condition pour s'assurer qu'on se trouve sur la bonne socket a lire et pas sur la socket du serveur qui na pas recu de connexions **/
				else
				{
					std::cout << "Right socket" << std::endl;
					char	buffer[255];
					int		len;
					std::string response;

					rc = recv(pollfds[y].fd, buffer, sizeof(buffer), 0);
					if (rc < 0)
						perror("Recv() failed");
					else
					{
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
						rc = send(pollfds[y].fd, response.c_str(), len, 0);
						if (rc < 0)
							perror("Send() failed");

						std::cout << "Message sent" << std::endl;
					}
				}
				close(pollfds[y].fd);
			}
		}
	}
	for (int x = 0; x < 2; x++)
		close(server_fd[x]);
	return (0);
}
