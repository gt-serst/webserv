#include "Response.hpp"
#include <sys/stat.h>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <sstream>

static std::string intToString(int number)
{
	std::ostringstream oss;
	oss << number;
	return (oss.str());
}

void	Response::handleCGI(std::string rootedpath, std::string path, Request& req)
{
	std::map<std::string, std::string>		cgi_path = req._server.getConfig().cgi_path;
	std::map<std::string, std::string>::iterator	it = cgi_path.begin();
	struct stat					sb;
	//char*						content_length = sprintf(content_length, "%d", req.getLen());;
	//char*						client_host = sprintf(client_host, "%d", req.getHost());
	while (it != cgi_path.end())
	{
		if (path.compare(path.length() - it->first.length(), it->first.length(), it->first) == 0 && it->second.empty() == 0)
		{
			if (stat(it->second.c_str(), &sb) == 0 && access(it->second.c_str(), X_OK) == 0)
			{
				if (stat(rootedpath.c_str(), &sb) == 0 && access(rootedpath.c_str(), X_OK) == 0)
				{
					if (fork() == 0)
					{
						int fd = req._server._requests.begin()->first;
						if (dup2(fd, STDIN_FILENO) != -1 || dup2(fd, STDOUT_FILENO) != -1)
						{
							std::string path_info = "PATH_INFO=" + path;
							std::string path_translated = "PATH_TRANSLATED=" + rootedpath;
							std::string query_string = "QUERY_STRING=" + req.getQuerystr();
							std::string content_length = "CONTENT_LENGTH=" + req.getLen();
							std::string content_type = "CONTENT_TYPE=" + req.getHeader("Content-Type");
							std::string remote_addr = "REMOTE_ADDR=" + req.getIp();
							std::string remote_host = "REMOTE_HOST=" + req.getHost();
							std::string request_method = "REQUEST_METHOD=" + req.getRequestMethod();
							std::string server_name = "SERVER_NAME=" + req.getHost();
							std::string server_port = "SERVER_PORT=" + intToString(req._server.getConfig().port);
							std::string server_protocol = "SERVER_PROTOCOL=HTML/1.1";
							char *envp[] =
							{
								strdup(path_info.c_str()),
								strdup(path_translated.c_str()),
								strdup(query_string.c_str()),
								strdup(content_length.c_str()),
								strdup(content_type.c_str()),
								strdup(remote_addr.c_str()),
								strdup(remote_host.c_str()),
								strdup(request_method.c_str()),
								strdup(server_name.c_str()),
								strdup(server_port.c_str()),
								strdup(server_protocol.c_str()),
								0
							};
							char *argv[] =
							{
								strdup(rootedpath.c_str()),
								0
							};
							if (execve(it->second.c_str(), argv, envp) == -1)
								std::cerr << "ERROR : failed to execute CGI script" << std::endl;
						}
					}
					else
						std::cerr << "ERROR : fork() failed to open a new process" << std::endl;
					wait(NULL);
				}
				else
					std::cerr << "ERROR : path to " << rootedpath << " executable is not accessible." << std::endl;
			}
			else
				std::cerr << "ERROR : path to " << it->first << " executable is not accessible." << std::endl;
		}
		it++;
	}
}
