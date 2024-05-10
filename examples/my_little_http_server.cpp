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

	struct pollfd	pollfd_clients[MAX_FD];
	struct pollfd	pollfd_servers[MAX_FD];
	memset(pollfd_clients, 0, sizeof(pollfd_clients));
	memset(pollfd_servers, 0, sizeof(pollfd_servers));

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

		int	connection_backlog = 50;
		rc = listen(server_fd[x], connection_backlog);
		if (rc < 0)
			perror("Listen() failed");

		pollfd_servers[x].fd = server_fd[x];
		pollfd_servers[x].events = POLLIN;
	}

	struct sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);

	/*rc = poll(pollfd_servers, MAX_FD, -1);
	if (rc < 0)
		perror("Poll() failed");*/
	bool pollused = false;
	bool writing = false;
	while (1)
	{
		std::cout << "New loop" << std::endl;
		rc = poll(pollfd_servers, MAX_FD, -1);
		for (int x = 0; x < 2; x++)
		{
			std::cout << "Listening loop" << std::endl;
			/*if (start == true)
			{
				std::cout << "Out-loop Poll Initialization" << std::endl;
				rc = poll(pollfd_clients, MAX_FD, -1);
			}*/
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

				for (int z = 0; z < MAX_FD; z++) 
				{
					std::cout << "Poll elem initialization loop" << std::endl;
					std::cout << pollfd_clients[z].fd << std::endl;
					if (pollfd_clients[z].fd == 0 && z == 0)
					{
						std::cout << "First elem initialization" << std::endl;
						pollfd_clients[z].fd = client_fd;
						pollfd_clients[z].events = POLLIN;
						std::cout << "In-loop Poll initialization" << std::endl;
						rc = poll(pollfd_clients, MAX_FD, -1);
						if (rc < 0)
							perror("Poll() failed");
						pollused = true;
						writing = false;
						break;
					}
					else if (pollfd_clients[z].fd == 0)
					{
						std::cout << "Elem initialization" << std::endl;
						pollfd_clients[z].fd = client_fd;
						pollfd_clients[z].events = POLLIN;
						pollused = true;
						writing = false;
						break;
					}
				}
				for (int y = 0; y < MAX_FD; y++)
				{
					std::cout << "Writing loop" << std::endl;
					if (pollfd_clients[y].revents & POLLIN)
					{
						char	buffer[255];
						int		len;
						std::string response;

						std::cout << "Waiting to write" << std::endl;
						rc = recv(pollfd_clients[y].fd, buffer, sizeof(buffer), 0);
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
							rc = send(pollfd_clients[y].fd, response.c_str(), len, 0);
							if (rc < 0)
								perror("Send() failed");
						}
						close(pollfd_clients[y].fd);
						writing = true;
					}
				}
			}
			if (pollused == true && writing == true)
				rc = poll(pollfd_clients, MAX_FD, -1);
			pollused = false;
		}
		//rc = poll(pollfd_clients, MAX_FD, -1);
		std::cout << "End loop" << std::endl;

		/*for (int x = 0; x < 2; x++)
		{
			for (int y = 0; y < MAX_FD; y++)
			{

				 //Condition pour s'assurer que l'event qui a eu lieu survient bien sur le serveur ou la boucle se situe actuellement
				if (!(pollfd_servers[x].revents & POLLIN) && !(pollfd_clients[y].revents & POLLIN))
					continue;
				if ((pollfd_servers[x].revents & POLLIN) && !(pollfd_clients[y].revents & POLLIN))
				{
					std::cout << "New connection" << std::endl;
					client_fd = accept(pollfd_servers[x].fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);

					// Condition qui rejette tous les serveurs qui n'ont pas recu de connexions	
					if (client_fd < 0)
						perror("Accept() failed");

					flags = fcntl(client_fd, F_GETFL, 0);
					if (flags < 0)
						perror("Fcntl() failed");
					fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

					for (int z = 0; z < MAX_FD; z++) 
					{
						if (pollfd_clients[z].fd == 0 && z == 0)
						{
							std::cout << "z == 0" << std::endl;
							pollfd_clients[z].fd = client_fd;
							pollfd_clients[z].events = POLLIN;
							rc = poll(pollfd_clients, MAX_FD, -1);
							if (rc < 0)
								perror("Poll() failed");
							break;
						}
						else if (pollfd_clients[z].fd == 0)
						{
							pollfd_clients[z].fd = client_fd;
							pollfd_clients[z].events = POLLIN;
							break;
						}
					}
				}

				// Condition pour s'assurer qu'on se trouve sur la bonne socket a lire et pas sur la socket du serveur qui na pas recu de connexions
				else
				{
					std::cout << "Right socket" << std::endl;
					char	buffer[255];
					int		len;
					std::string response;

					std::cout << pollfd_clients[y].fd << std::endl;
					std::cout << pollfd_clients[y].revents << std::endl;
					rc = recv(pollfd_clients[y].fd, buffer, sizeof(buffer), 0);
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
						rc = send(pollfd_clients[y].fd, response.c_str(), len, 0);
						if (rc < 0)
							perror("Send() failed");

						std::cout << "Message sent" << std::endl;
					}
				}
				//close(pollfd_clients[y].fd);
			}
		}*/
	}
	for (int x = 0; x < 2; x++)
		close(pollfd_servers[x].fd);
	return (0);
}
