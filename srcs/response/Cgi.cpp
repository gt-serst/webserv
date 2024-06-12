#include "Response.hpp"
#include <sys/stat.h>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <sstream>
#include <sys/wait.h>
#include <sys/types.h>
#include <iostream>
#include <map>
#include <cstring>

static std::string intToString(int number) {
	std::ostringstream oss;
	oss << number;
	return oss.str();
}

void Response::handleCGI(std::string rootedpath, std::string path, Request& req, std::string exec_path) {
	struct stat sb;

	if (stat(exec_path.c_str(), &sb) == 0 && access(exec_path.c_str(), X_OK) == 0) {
		if (stat(rootedpath.c_str(), &sb) == 0 && access(rootedpath.c_str(), X_OK) == 0) {
			pid_t p = fork();
			if (p == 0) { // Child process
				int client_fd = req._server.getClientFd();

				// Redirect stdout to client_fd
				if (dup2(client_fd, STDOUT_FILENO) == -1) {
					std::cerr << "ERROR: dup2() failed" << std::endl;
					exit(EXIT_FAILURE);
				}

				std::string path_info = "PATH_INFO=" + path;
				std::string path_translated = "PATH_TRANSLATED=" + rootedpath;
				std::string query_string = "QUERY_STRING=" + req.getQuerystr();
				std::string content_length = "CONTENT_LENGTH=" + intToString(req.getLen());
				std::string content_type = "CONTENT_TYPE=" + req.getHeader("Content-Type");
				std::string remote_addr = "REMOTE_ADDR=" + req.getIp();
				std::string remote_host = "REMOTE_HOST=" + req.getHost();
				std::string request_method = "REQUEST_METHOD=" + req.getRequestMethod();
				std::string server_name = "SERVER_NAME=" + req.getHost();
				std::string server_port = "SERVER_PORT=" + intToString(req._server.getConfig().port);
				std::string server_protocol = "SERVER_PROTOCOL=HTTP/1.1";

				char *envp[] = {
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
					NULL
				};

				char *argv[] = {
					strdup(exec_path.c_str()),
					strdup(rootedpath.c_str()),
					NULL
				};
				if (execve(exec_path.c_str(), argv, envp) == -1) {
					std::cerr << "ERROR: Failed to execute CGI script" << std::endl;
					perror("execve");
					exit(EXIT_FAILURE);
				}

				// Free dynamically allocated memory
				for (size_t i = 0; envp[i]; ++i) {
					free(envp[i]);
				}
				for (size_t i = 0; argv[i]; ++i) {
					free(argv[i]);
				}
			} else if (p == -1) { // Fork failed
				std::cerr << "ERROR: fork() failed to open a new process" << std::endl;
				return;
			} else { // Parent process
				int status;
				pid_t result = waitpid(p, &status, 0);
				if (result == -1) {
					std::cerr << "ERROR: waitpid() failed" << std::endl;
				} else {
					if (WIFEXITED(status)) {
						std::cout << "Child exited with status " << WEXITSTATUS(status) << std::endl;
					} else if (WIFSIGNALED(status)) {
						std::cout << "Child killed by signal " << WTERMSIG(status) << std::endl;
					} else if (WIFSTOPPED(status)) {
						std::cout << "Child stopped by signal " << WSTOPSIG(status) << std::endl;
					}
				}
			}
		} else {
			std::cerr << "ERROR: Path to " << rootedpath << " executable is not accessible." << std::endl;
		}
	} else {
		std::cerr << "ERROR: Path to " << exec_path << " executable is not accessible." << std::endl;
	}
}
