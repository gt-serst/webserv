#include "Response.hpp"
#include <sys/stat.h>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <sstream>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <iostream>
#include <map>
#include <cstring>
#include <signal.h>
#include <fcntl.h>

static std::string intToString(int number) {
    std::ostringstream oss;
    oss << number;
    return oss.str();
}

void Response::handleCGI(std::string rootedpath, std::string path, Request& req, std::string exec_path) {
    struct stat sb;

    if (stat(exec_path.c_str(), &sb) == 0 && access(exec_path.c_str(), X_OK) == 0) {
        if (stat(rootedpath.c_str(), &sb) == 0 && access(rootedpath.c_str(), X_OK) == 0) {
            int pipefd[2];
            if (pipe(pipefd) == -1) {
                std::cerr << "ERROR: CGI: pipe() failed" << std::endl;
                return;
            }

            pid_t p = fork();
            int client_fd = req._server.getClientFd();
            if (p == 0) { // Child process
                close(pipefd[0]); // Close unused read end
                // Redirect stdout to pipe
                if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
                    std::cerr << "ERROR: CGI: dup2() failed" << std::endl;
                    exit(EXIT_FAILURE);
                }
                close(pipefd[1]); // Close write end after dup

                std::string path_info = "PATH_INFO=" + path;
                std::string path_translated = "PATH_TRANSLATED=" + rootedpath;
                std::string query_string = "QUERY_STRING=" + req.getQuerystr();
                std::string content_length = "CONTENT_LENGTH=" + intToString(req.getLen());
                std::string content_type = "CONTENT_TYPE=" + req.getHeader("Content-Type");
                std::string remote_host = "REMOTE_HOST=" + intToString((int)req._server.getClientAddr().sin_port);
                std::string remote_addr = "REMOTE_ADDR=" + intToString((int)req._server.getClientAddr().sin_addr.s_addr);
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
                    std::cerr << "ERROR: CGI: Failed to execute CGI script" << std::endl;
                    perror("execve");
                }

                // Free dynamically allocated memory
                for (size_t i = 0; envp[i]; ++i) {
                    free(envp[i]);
                }
                for (size_t i = 0; argv[i]; ++i) {
                    free(argv[i]);
                }
                exit(EXIT_FAILURE);
            } else if (p == -1) { // Fork failed
                std::cerr << "ERROR: CGI: fork() failed to open a new process" << std::endl;
                close(pipefd[0]);
                close(pipefd[1]);
                return;
            } else { // Parent process
                close(pipefd[1]); // Close unused write end

                int status;
                fd_set rfds;
                struct timeval tv;
                FD_ZERO(&rfds);
                FD_SET(pipefd[0], &rfds);
                tv.tv_sec = 5; // Timeout in seconds
                tv.tv_usec = 0;

                int retval = select(pipefd[0] + 1, &rfds, NULL, NULL, &tv);
                if (retval == -1) {
                    std::cerr << "ERROR: CGI: select() failed" << std::endl;
                    perror("select()");
                } else if (retval == 0) {
                    std::cerr << "ERROR: CGI: Timeout reached, terminating CGI script" << std::endl;
                    kill(p, SIGKILL);
                } else {
                    char buffer[1024];
                    ssize_t bytesRead;
                    while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
                        write(client_fd, buffer, bytesRead);
                    }
                }
                close(pipefd[0]);

                pid_t result = waitpid(p, &status, 0);
                if (result == -1) {
                    std::cerr << "ERROR: CGI: waitpid() failed" << std::endl;
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
            std::cerr << "ERROR: CGI: Path to " << rootedpath << " executable is not accessible." << std::endl;
        }
    } else {
        std::cerr << "ERROR: CGI: Path to " << exec_path << " executable is not accessible." << std::endl;
    }
}

