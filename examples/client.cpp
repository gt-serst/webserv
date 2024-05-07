#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define PORT "3490"
#define MAXDATASIZE 100

int	main(int argc, char **argv)
{
	int		sockfd;
	int		numbytes;
	char		buf[MAXDATASIZE];
	int		status;
	struct addrinfo	hints;
	struct addrinfo	*servinfo;
	struct addrinfo	*p;

	if (argc != 2)
	{
		std::cerr << "usage: client hostname" << std::endl;
		exit (1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	if ((status = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
	{
		std::cerr << "getaddrinfo error:" << gai_strerror(status) << std::endl;
		exit(1);
	}

	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			perror("client: socket");
			continue;
		}
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("client: connect");
			continue;
		}
		break;
	}
	
	if (p == NULL)
	{
		std::cerr << "client: failed to connect" << std::endl;
		exit(1);
	}
	
	
	freeaddrinfo(servinfo);
	
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
	{
		perror("recv");
		exit(1);
	}
	
	buf[numbytes] = '\0';
	
	std::cout << "client: received " << buf << std::endl;	

	close(sockfd);
	
	return (0);
}
