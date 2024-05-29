/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 16:28:16 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/28 17:52:31 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "../parser/confParser.hpp"
#include "../request/Request.hpp"
#include "Router.hpp"
#include <string>
#include <map>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <sstream>

Response::Response(){}

Response::~Response(){}

void	Response::handleDirective(std::string path, t_locations loc, std::map<std::string, t_locations> routes, Request req, std::map<int, std::string> error_paths){

	this->_http_version = req.getVersion();
	this->_status_code = 404;
	this->_status_message = "Not Found";

	(void)error_paths;
	if (attachRootToPath(path, loc.root_path) == false)
		perror("404 Root failed");
	else
	{
		if (getFileType(path) == E_DIR)
		{
			if (path[path.length() - 1] != '/')
				loc.root_path.insert(loc.root_path.length() - 1, "/");
			if (findIndexFile(path, loc, routes) == true)
			{
				attachRootToPath(path, loc.root_path); //path du fichier index doit-il encore garder le chemin du répertoire dans lequel il se trouve, pour le moment le path, avant d'être rooté, correspond uniquement au fichier index
				fileRoutine(path, loc, req);
			}
			else if (isMethodAllowed(loc, req) == true)
				runDirMethod(path, loc, req);
			else
				perror("405 Method Not Allowed");
		}
		else if (getFileType(path) == E_FILE)
			fileRoutine(path, loc, req);
		else
			perror("404 Neither a dir nor a file");
	}
}

bool	Response::attachRootToPath(std::string& path, std::string root){

	if (root.empty() == false)
	{
		if (root[root.length() - 1] == '/')
			root.erase(root.length() - 1,1);
		std::cout << "Path: " << path << std::endl;
		path.insert(0, root);
		std::cout << "Path rooted: " << path << std::endl;
	}
	else
		return (false);
	return (true);
}

int	Response::getFileType(std::string path){

	struct stat path_stat;

	if (stat(path.c_str(), &path_stat) != 0)
		perror("404 Stat failed");
	if (S_ISDIR(path_stat.st_mode) == true)
	{
		std::cout << "Path is a directory" << std::endl;
		return (E_DIR);
	}
	else if (S_ISREG(path_stat.st_mode) == true)
	{
		std::cout << "Path is a file" << std::endl;
		return (E_FILE);
	}
	else
		return (E_UNKNOWN);
}

bool	Response::findIndexFile(std::string& path, t_locations& loc, std::map<std::string, t_locations> routes){

	std::string	index;
	Router		router;

	if(loc.default_path.empty() == false)
	{
		for (size_t i = 0; i < loc.default_path.size(); i++)
		{
			index = path;
			index.append(loc.default_path[i]);
			if (access(index.c_str(), F_OK) == 0)
			{
				path = loc.default_path[i].insert(0, "/");
				loc = router.routeRequest(path, routes);
				return (true);
			}
		}
		perror("403 Index failed");
	}
	return (false);
}

void	Response::fileRoutine(std::string path, t_locations loc, Request req){

	if (findCGI(req._server.getConfig().cgi_path) == true && isMethodAllowed(loc, req) == true)
		std::cout << "Send CGI path and run it" << std::endl;
	else if (isMethodAllowed(loc, req) == true)
		runFileMethod(path, req);
	else
		perror("405 Method Not Allowed");
}

bool	Response::findCGI(std::string cgi_path){

	if (cgi_path.empty() == false)
		return (true);
	else
		return (false);
}

bool	Response::isMethodAllowed(t_locations loc, Request req){

	for (size_t i = 0; i < loc.allowed_methods.size(); i++)
	{
		if (loc.allowed_methods[req.getRequestMethod()] == true)
			return (true);
	}
	return (false);
}

void	Response::runDirMethod(std::string path, t_locations loc, Request req){

	if (req.getRequestMethod() == "GET")
		isAutoIndex(path, loc);
	else if (req.getRequestMethod() == "POST")
		uploadDir(path);
	else if (req.getRequestMethod() == "DELETE")
		deleteDir(path);
}

void	Response::isAutoIndex(std::string path, t_locations loc){

	::DIR			*dr;
	struct dirent	*de;
	std::string		dir_list;

	if ((dr = opendir(path.c_str())) != NULL)
	{
		if (loc.auto_index == true)
		{
			while ((de = readdir(dr)) != NULL)
				dir_list.append(de->d_name);
			autoIndexResponse(dir_list);
			closedir(dr);
		}
		else
		{
			closedir(dr);
			perror("403 Autoindex failed");
		}
	}
	else
		perror("404 Opendir failed");
}

void	Response::uploadDir(std::string path){

	if (access(path.c_str(), W_OK) == 0)
	{
		uploadDirResponse();
	}
	else
		perror("403 Write access failed");
}

void	Response::deleteDir(std::string path){

	if (access(path.c_str(), W_OK) == 0)
	{
		if (rmdir(path.c_str()) < 0)
			perror("500 Delete directory failed");
		deleteResponse();
	}
	else
		perror("403 Write access failed");
}

void	Response::runFileMethod(std::string path, Request req){

	if (req.getRequestMethod() == "GET")
		downloadFile(path);
	else if (req.getRequestMethod() == "POST")
		uploadFile(path, req);
	else if (req.getRequestMethod() == "DELETE")
		deleteFile(path);
}

void	Response::downloadFile(std::string path){

	std::ifstream input(path, std::ios::binary);

	if (input.is_open())
	{
		std::string buffer;
		std::string stack;
		while (std::getline(input, buffer))
		{
			stack += buffer;
			stack += '\n';
		}
		input.close();
		downloadFileResponse(stack);
	}
	else
		perror("404 Open failed");
}

void	Response::uploadFile(std::string path, Request req){

	std::ofstream output(path);

	if (output.is_open())
	{
		output << req.getBody();
		output.close();
		uploadFileResponse();
	}
	else
		perror("404 File creation failed");
}

void	Response::deleteFile(std::string path){

	if (access(path.c_str(), W_OK) == 0)
	{
		if (remove(path.c_str()) < 0)
			perror("404 Delete file failed");
		deleteResponse();
	}
	else
		perror("403 Write access failed");
}

void	Response::autoIndexResponse(std::string dir_list){

	(void)dir_list;
	//ne pas pv revenir derrière le root
}

void	Response::deleteResponse(void){

	this->_status_code = 200;
	this->_status_message = "OK";
	generateResponse();
}

void	Response::uploadDirResponse(void){

	this->_status_code = 201;
	this->_status_message = "Created";
	generateResponse();
}

void	Response::downloadFileResponse(std::string stack){

	this->_status_code = 200;
	this->_status_message = "OK";
	this->_content_type = getContentType(stack);
	this->_content_len = stack.length();
	this->_body = stack;
	generateResponse();
}

std::string	Response::getContentType(std::string stack){

	std::stringstream	ss;
	std::stringstream	ss_hex;
	std::vector<char>	bytes(8);
	std::string			octets;

	if (stack.compare(0, 15, "<!DOCTYPE html>") == 0)
		return ("text/html");
	ss << stack;
	ss.read(&bytes[0], bytes.size());
	for (size_t i = 0; i < bytes.size(); i++)
		ss_hex << std::hex << (static_cast<int>(bytes[i]) & 0xFF) << " ";
	octets = ss_hex.str();
	switch (stringToEnum(octets))
	{
		case PNG:
			return ("image/png");
		case JPEG:
			return ("image/jpeg");
		case SVG:
			return ("image/svg+xml");
		case GIF:
			return ("image/gif");
		case PDF:
			return ("application/pdf");
		case ZIP:
			return ("application/zip");
		case MP4:
			return ("video/mp4");
		case TXT:
			return ("text/plain");
		default:
			perror("No file recognised");
	}
}

t_file_type	Response::stringToEnum(std::string const& str){

	std::cout << str << std::endl;
	if (str.compare(0, 20, "89 50 4e 47 d a 1a a") == 0) return (PNG);
	if (str.compare(0, 5, "ff d8") == 0) return (JPEG);
	if (str.compare(0, 23, "3c 3f 78 6d 6c 20 76 65") == 0) return (SVG);
	if (str.compare(0, 17, "47 49 46 38 39 61") == 0) return (GIF);
	if (str.compare(0, 11, "25 50 44 46") == 0) return (PDF);
	if (str.compare(0, 18, "50 4b 3 4 14 0 8 0") == 0) return (ZIP);
	if (str.compare(0, 20, "0 0 0 20 66 74 79 70") == 0) return (MP4);
	else
		return (TXT);
}

void	Response::uploadFileResponse(void){

	this->_status_code = 201;
	this->_status_message = "Created";
	generateResponse();
}

void	Response::errorResponse(int error_code, std::string message, std::map<int, std::string> error_paths){

	std::string path;

	path = matchErrorCodeWithPage(error_code, error_paths);

	this->_status_code = error_code;
	this->_status_message = message;
	this->_content_type = "text/html";

	std::ifstream	input(path);
	if (input.is_open())
	{
		std::string	buffer;
		std::string	stack;

		while (std::getline(input, buffer))
		{
			stack += buffer;
			stack += '\n';
		}
		input.close();
		this->_content_len = stack.length();
		this->_body = stack;
		generateResponse();
	}
	else
	{
		error_paths.clear();
		errorResponse(404, "Not Found", error_paths);
	}
}

std::string	Response::matchErrorCodeWithPage(int error_code, std::map<int, std::string> error_paths){

	for (std::map<int, std::string>::iterator it = error_paths.begin(); it != error_paths.end(); ++it)
	{
		if (it->first == error_code)
			return (it->second);
	}
	if (error_code == 400)
		return ("/Users/gt-serst/webserv/var/www/html/error400.html");
	else
		return ("/Users/gt-serst/webserv/var/www/html/error404.html");
}

void	Response::generateResponse(void){

	if (this->_body.empty() == false)
	{
		this->_response = std::string("HTTP/1.1") + std::string(" ") +\
			std::to_string(this->_status_code) + std::string(" ") +\
			this->_status_message + std::string("\r\n") +\
			std::string("Content-Type: ") + this->_content_type + std::string("\r\n") +\
			std::string("Content-Length: ") + std::to_string(this->_content_len) + std::string("\r\n\r\n") +\
			this->_body + std::string("\r\n\r\n");
	}
	else
	{
		this->_response = std::string("HTTP/") +\
			this->_http_version + std::string(" ") +\
			std::to_string(this->_status_code) + std::string(" ") +\
			this->_status_message + std::string("\r\n\r\n\r\n\r\n");
	}
}

std::string	Response::getResponse(void) const{

	return _response;
}
