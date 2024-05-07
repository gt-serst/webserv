#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <poll.h>
#include <fcntl.h>
#include <curses.h>

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
					char	buffer[255];
					int		len;

					rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
					if (rc < 0)
						break;
					if (rc == 0)
						break;
					std::cout << buffer << std::endl;
					len = rc;
					rc = send(fds[i].fd, buffer, len, 0);
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
