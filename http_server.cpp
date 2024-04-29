#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fstream>
#include <string.h>

bool is_path_empty(const std::string& request)
{
	size_t slash_position = request.find('/');

	return isspace(request[slash_position + 1]);
}

std::string	parse_echo(std::string buffer)
{
	std::string rline;

	size_t start = buffer.find("/echo/");
	size_t end = buffer.find(" ", start + 6);
	if (start != std::string::npos && end != std::string::npos)
	{
		start += 6;
		rline = buffer.substr(start, end - start);
	}

	return (rline);
}

std::string	parse_header(std::string buffer)
{
	std::string uagent;

	size_t start = buffer.find("User-Agent:");
	size_t end = buffer.find("\r\n", start + 12);
	if (end == std::string::npos)
		end = buffer.length();
	if (start != std::string::npos)
	{
		start += 12;
		uagent = buffer.substr(start, end - start);
	}
	return (uagent);
}

bool	parse_user(std::string buffer)
{
	buffer.erase(0, buffer.find_first_of("/") + 1);
	buffer.erase(buffer.find_first_of(" "), buffer.length());
	if (buffer == std::string("user-agent"))
		return (true);
	else
		return (false);
}

std::string	parse_file(std::string buffer)
{
	std::string rline;

	size_t start = buffer.find("/files/");
	size_t end = buffer.find(" ", start + 7);
	if (start != std::string::npos && end != std::string::npos)
	{
		start += 7;
		rline = buffer.substr(start, end - start);
	}

	return (rline);
}

char	*read_input_file(std::string dir, std::string file_name)
{
	int				length;
	std::ifstream	ifs;

	dir.append(file_name);
	ifs.open(dir, std::ifstream::binary);
	if (ifs)
	{
		ifs.seekg (0, ifs.end);
		length = ifs.tellg();
		ifs.seekg (0, ifs.beg);
	}
	else
	{
		std::cout << "Open error" << std::endl;
		return (NULL);
	}
	char *buffer = new char [length];
	ifs.read (buffer, length);
	if (!ifs)
	{
		std::cout << "Read error" << std::endl;
		return (NULL);
	}
	ifs.close();
	return (buffer);
}

bool	write_output_file(std::string dir, std::string file_name, std::string content)
{
	std::ofstream ofs(dir.append(file_name));
	if (ofs.is_open())
	{
		ofs << content;
		ofs.close();
	}
	else
	{
		std::cout << "error: failed to create the file" << std::endl;
		return (false);
	}
	return (true);
}

std::string	get_content_from_body(std::string buffer)
{
	std::string content;

	size_t start = buffer.find_last_of("\r\n");
	if (start != std::string::npos)
	{
		start += 1;
		content = buffer.substr(start, buffer.length() - start);
	}
	return (content);
}

int main(int argc, char **argv) {

	int			server_fd;
	int			client_fd;
	pid_t		childpid;
	std::string	dir;

	if (argc == 3 && strcmp(argv[1], "--directory") == 0)
		dir = argv[2];

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		std::cerr << "Failed to create server socket\n";
		return 1;
	}

	int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
		std::cerr << "setsockopt failed\n";
		return 1;
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(4221);

	if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
		std::cerr << "Failed to bind to port 4221\n";
		return 1;
	}

	int connection_backlog = 5;
	if (listen(server_fd, connection_backlog) != 0) {
		std::cerr << "listen failed\n";
		return 1;
	}

	struct sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);

	std::cout << "Waiting for a client to connect...\n";

	 int cnt = 0;
	while (1)
	{
		client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
		std::cout << "Client connected\n";

		std::cout << "Clients connected: " << cnt++ << std::endl;

		if ((childpid = fork()) == 0)
		{
			char buffer[255];
			int	len;

			len = read(client_fd, buffer, 255);
			if (len < 0)
				std::cout << "Failed to read the request" << std::endl;

			std::string response = "HTTP/1.1 404 Not Found\r\n\r\n";
			std::string body;

			if (is_path_empty(buffer))
				response = "HTTP/1.1 200 OK\r\n\r\n";
			else if (!parse_echo(buffer).empty())
			{
				body = parse_echo(buffer);
				response = std::string("HTTP/1.1 200 OK\r\n") + std::string("Content-Type: text/plain\r\nContent-Length: ") + std::to_string(body.length()) + std::string("\r\n\r\n") + body + "\r\n\r\n";
			}
			else if (parse_user(buffer) && !parse_header(buffer).empty())
			{
				body = parse_header(buffer);
				response = std::string("HTTP/1.1 200 OK\r\n") + std::string("Content-Type: text/plain\r\nContent-Length: ") + std::to_string(body.length()) + std::string("\r\n\r\n") + body + "\r\n\r\n";
			}
			else if (strncmp(buffer, "GET", 3) == 0 && !parse_file(buffer).empty() && read_input_file(dir, parse_file(buffer)))
			{
				body = read_input_file(dir, parse_file(buffer));
				response = std::string("HTTP/1.1 200 OK\r\n") + std::string("Content-Type: application/octet-stream\r\nContent-Length: ") + std::to_string(body.length()) + std::string("\r\n\r\n") + body + "\r\n\r\n";
			}
			else if (strncmp(buffer, "POST", 4) == 0 && !parse_file(buffer).empty() && write_output_file(dir, parse_file(buffer), get_content_from_body(buffer)))
			{
				response = std::string("HTTP/1.1 201 OK\r\n\r\n");
			}

			if (write(client_fd, response.c_str(), response.length()) < 0)
				std::cout << "Failed to write the request" << std::endl;

			std::cout << "Message send" << std::endl;
			close(client_fd);
			exit(0);
		}
		close(client_fd);
	}

	close(server_fd);

	return (0);
}

/*int main(int argc, char **argv)
{
	std::string	dir;

	if (argc == 3 && strcmp(argv[1], "--directory") == 0)
		dir = argv[2];

	std::string buffer = "POST /files/Coo_donkey_dumpty_Horsey HTTP/1.1\r\nHost: localhost:4221\r\nUser-Agent: Go-http-client/1.1\r\nContent-Length: 48\r\nAccept-Encoding: gzip\r\n\r\nyikes Coo dumpty Horsey 237 scooby scooby dumpty";

	std::string response = "HTTP/1.1 404 Not Found\r\n\r\n";
	std::string body;

	if (is_path_empty(buffer))
		response = "HTTP/1.1 200 OK\r\n\r\n";
	else if (!parse_echo(buffer).empty())
	{
		body = parse_echo(buffer);
		response = std::string("HTTP/1.1 200 OK\r\n") + std::string("Content-Type: text/plain\r\nContent-Length: ") + std::to_string(body.length()) + std::string("\r\n\r\n") + body + "\r\n\r\n";
	}
	else if (parse_user(buffer) && !parse_header(buffer).empty())
	{
		body = parse_header(buffer);
		response = std::string("HTTP/1.1 200 OK\r\n") + std::string("Content-Type: text/plain\r\nContent-Length: ") + std::to_string(body.length()) + std::string("\r\n\r\n") + body + "\r\n\r\n";
	}
	else if (strncmp(buffer.c_str(), "GET", 3) == 0 && !parse_file(buffer).empty() && read_input_file(dir, parse_file(buffer)))
	{
		body = read_input_file(dir, parse_file(buffer));
		response = std::string("HTTP/1.1 200 OK\r\n") + std::string("Content-Type: application/octet-stream\r\nContent-Length: ") + std::to_string(body.length()) + std::string("\r\n\r\n") + body + "\r\n\r\n";
	}
	else if (strncmp(buffer.c_str(), "POST", 4) == 0 && !parse_file(buffer).empty() && write_output_file(dir, parse_file(buffer), get_content_from_body(buffer)))
	{
		response = std::string("HTTP/1.1 201 OK\r\n\r\n");
	}

	std::cout << response << std::endl;

	std::cout << "Message send" << std::endl;

	return (0);
}*/
