/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 16:28:16 by gt-serst          #+#    #+#             */
/*   Updated: 2024/06/07 12:48:39 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "../parser/confParser.hpp"
#include "../request/Request.hpp"
#include "Router.hpp"
#include <string>
#include <map>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <sstream>
#include <ctime>

Response::Response(void){}

Response::~Response(void){}

bool	Response::checkPortAndServerName(t_server_scope config){

	if (config.port < 0 || config.server_name.empty() == true)
		return (false);
	return (true);
}

bool	Response::checkContentType(std::string path){

	std::ifstream input(path, std::ios::binary);

	std::cout << "Path: " << path << std::endl;
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
		std::string type = getContentType(stack);
		std::cout << "Type: "<< type << std::endl;
		if (type == "text/html" || type == "text/plain" || type == "image/png" || type == "image/jpeg" || type == "image/svg+xml")
			return (true);
	}
	std::cout << "<<< Wrong Type >>>" << std::endl;
	return (false);
}

void	Response::handleDirective(std::string path, t_locations loc, Request& req, Server& serv){

	std::string					rooted_path;
	std::map<int, std::string>	rooted_error_paths;

	// if (req.getRequestMethod() == "POST")
	// 	uploadFile(path, loc.root_path, serv.getConfig().upload_path, req, serv);

	rooted_error_paths = serv.getConfig().error_page_paths;
	// for (std::map<int, std::string>::iterator it = rooted_error_paths.begin(); it != rooted_error_paths.end(); ++it)
	// {
	// 	if (attachRootToPath(it->second, loc.root_path) == false)
	// 		return;
	// 	std::cout << "Rooted error_path: " << it->second << std::endl;
	// }
	if (rootPaths(loc, path, serv.getConfig().upload_path, rooted_error_paths, req) == true)
	{
		//if (attachRootToPath(path, loc.root_path, serv.getConfig().error_page_paths) == true)
		//{
			// in the case of a POST request we should not check if the type file because we only accept file to be uploaded on the server, and the last part of the path refers to the file name to be created so obviously it does not yet exist
			struct stat buf;

			rooted_path = path;
			if (stat(path.c_str(), &buf) != 0)
			{
				std::cout << "Rooted_path when stat failed: " << rooted_path << std::endl;
				errorResponse(404, "Not Found", rooted_error_paths);
			}
			else if (getFileType(buf) == 0)
			{
				std::cout << "Dir" << std::endl;
				if (rooted_path.find_last_of("/") != rooted_path.length() - 1)
					rooted_path.insert(rooted_path.length(), "/");
				if (findDefaultFile(rooted_path, loc, serv.getConfig().locations, req) == true)
				{
					if (attachRootToPath(rooted_path, loc.root_path) == true)
					{	//check if the file is readable and that the content-type is accepted
						fileRoutine(rooted_path, rooted_error_paths, loc, req);
					}
				}
				else if (isMethodAllowed(loc, req) == true)
					runDirMethod(rooted_path, rooted_error_paths, loc, req);
				else
				{
					errorResponse(405, "Method Not Allowed", rooted_error_paths);
					perror("405 Method Not Allowed");
				}
			}
			else if (getFileType(buf) == 1)
			{
				std::cout << "File" << std::endl;
				//check if the file is readable and that the content-type is accepted
				fileRoutine(rooted_path, rooted_error_paths, loc, req);
			}
			else
			{
				errorResponse(415, "Unsupported Media Type", rooted_error_paths);
				perror("415 Unsupported Media Type");
			}
		//}
	}
	rooted_error_paths.clear();
}

bool	Response::rootPaths(t_locations loc, std::string& path, std::string upload_path, std::map<int, std::string>& rooted_error_paths, Request& req){

	for (std::map<int, std::string>::iterator it = rooted_error_paths.begin(); it != rooted_error_paths.end(); ++it)
	{
		if (attachRootToPath(it->second, loc.root_path) == false)
			return (false);
		std::cout << "Rooted error_path: " << it->second << std::endl;
	}
	if (req.getRequestMethod() == "POST")
	{
		std::string rooted_upload_path;

		if (attachRootToPath(upload_path, loc.root_path) == true)
		{
			//check if the file is readable and that the content-type is accepted, but for a POST method the content will be in the body, lets see how the browser will send a media type
			rooted_upload_path = upload_path;
			if (isMethodAllowed(loc, req) == true)
				uploadFile(path, rooted_upload_path, rooted_error_paths, req);
			else
			{
				errorResponse(405, "Method Not Allowed", rooted_error_paths);
				perror("405 Method Not Allowed");
			}
		}
		return (false);
	}
	else
		return (attachRootToPath(path, loc.root_path));
}

bool	Response::attachRootToPath(std::string& path, std::string root){

	if (root.empty() == false)
	{
		std::cout << root << std::endl;
		if (checkRootAccess(root) == true)
		{
			if (root[root.length() - 1] == '/')
				root.erase(root.length() - 1, 1);
			std::cout << "Path: " << path << std::endl;
			path.insert(0, root);
			std::cout << "Path rooted: " << path << std::endl;
			return (true);
		}
	}
	return (fileNotFound(), false);
}

int	Response::getFileType(struct stat buf){

	if (S_ISDIR(buf.st_mode) == true)
		return (0);
	else if (S_ISREG(buf.st_mode) == true)
		return (1);
	else
		return (-1);
}

bool	Response::findDefaultFile(std::string& path, t_locations& loc, std::map<std::string, t_locations> routes, Request& req){

	Router		router;

	if(loc.default_path.empty() == false)
	{
		for (int i = loc.default_path.size() - 1; i >= 0; i--)
		{
			std::string tmp = path;

			tmp.append(loc.default_path[i]);
			std::cout << tmp << std::endl;
			// if (checkFileAccess(path, error_paths) == true)
			if (access(tmp.c_str(), F_OK) == 0)
			{
				std::string	default_file;
				std::string new_path;

				new_path = req.getPathToFile();
				if (new_path.find_last_of("/") != new_path.length() - 1)
					new_path.insert(new_path.length(), "/");
				req.setPathToFile(new_path);
				path = req.getPathToFile().append(loc.default_path[i]);
				default_file = loc.default_path[i].insert(0, "/");
				std::cout << default_file << std::endl;
				if (router.routeRequest(default_file, loc, routes) == true)
				{
					req.setPathToFile(path);
					return (true);
				}
			}
		}
	}
	return (false);
}

void	Response::fileRoutine(std::string path, std::map<int, std::string> rooted_error_paths, t_locations loc, Request& req){

	if (checkFileAccess(path, rooted_error_paths) == true)
	{
		if (checkContentType(path) == true)
		{
			//détecter si l'extenssion du fichier demandé est un php, py etc pour lancer les bonnes CGI, itérer sur les chemins CGI pour lancer le script correspondant
			if (findCGI(req._server.getConfig().cgi_path) == true && isMethodAllowed(loc, req) == true)
				std::cout << "Send CGI path and run it" << std::endl;
			else if (isMethodAllowed(loc, req) == true)
				runFileMethod(path, rooted_error_paths, req);
			else
			{
				errorResponse(405, "Method Not Allowed", rooted_error_paths);
				perror("405 Method Not Allowed");
			}
		}
		else
		{
			errorResponse(415, "Unsupported Media Type", rooted_error_paths);
			perror("415 Unsupported Media Type");
		}

	}
}

bool	Response::findCGI(std::map<std::string, std::string>	cgi_path){

	if (cgi_path.empty() == false)
		return (true);
	else
		return (false);
}

bool	Response::isMethodAllowed(t_locations loc, Request& req){

	for (size_t i = 0; i < loc.allowed_methods.size(); i++)
	{
		if (loc.allowed_methods[req.getRequestMethod()] == true)
			return (true);
	}
	return (false);
}

void	Response::runDirMethod(std::string path, std::map<int, std::string> rooted_error_paths, t_locations loc, Request& req){

	if (req.getRequestMethod() == "GET")
	{
		if (loc.auto_index == true)
			isAutoIndex(path, rooted_error_paths, loc, req);
		else
			errorResponse(500, "Internal Server Error", rooted_error_paths);
	}
	else
		errorResponse(405, "Method Not Allowed", rooted_error_paths);
}

void	Response::isAutoIndex(std::string path, std::map<int, std::string> rooted_error_paths, t_locations loc, Request& req){

	::DIR			*dr;
	struct dirent	*de;
	std::string		dir_list;

	// if (access(path.c_str(), W_OK) == -1)
	// 	errorResponse(403, "Forbidden", error_paths);
	std::cout << "isAutoIndex function with path as dir: " << path << std::endl;
	if (checkFileAccess(path, rooted_error_paths) == true)
	{
		std::cout << "Try to opendir" << std::endl;
		if ((dr = opendir(path.c_str())) != NULL)
		{
			if (loc.auto_index == true)
			{
				while ((de = readdir(dr)) != NULL)
				{
					dir_list.append(de->d_name);
					dir_list += '\n';
				}
				autoIndexResponse(path, dir_list, req);
				closedir(dr);
			}
			else
			{
				closedir(dr);
				errorResponse(403, "Forbidden", rooted_error_paths);
				perror("403 Autoindex failed");
			}
		}
		else
		{
			errorResponse(404, "Not Found", rooted_error_paths);
			perror("404 Opendir failed");
		}
	}
}

void	Response::runFileMethod(std::string path, std::map<int, std::string> rooted_error_paths, Request& req){

	if (req.getRequestMethod() == "GET")
		downloadFile(path, rooted_error_paths);
	else if (req.getRequestMethod() == "DELETE")
		deleteFile(path, rooted_error_paths);
	else
		errorResponse(404, "Not Found", rooted_error_paths);
}

void	Response::downloadFile(std::string path, std::map<int, std::string> error_paths){

	std::ifstream input(path, std::ios::binary);

	//std::cout << path << std::endl;
	if (input.is_open())
	{
		//std::cout << "Open file" << std::endl;
		std::string buffer;
		std::string stack;
		while (std::getline(input, buffer))
		{
			stack += buffer;
			stack += '\n';
		}
		input.close();
		downloadFileResponse(stack, error_paths);
	}
	else
	{
		errorResponse(404, "Not Found", error_paths);
		perror("404 Open failed");
	}
}

void	Response::uploadFile(std::string path, std::string upload_path, std::map<int, std::string> rooted_error_paths, Request& req){

	// Root verification can perhaps be done in the main function
	if (upload_path[upload_path.length() - 1] == '/' && path[0] == '/')
		path.erase(0, 1);
	std::cout << "Upload file" << std::endl;
	path.insert(0, upload_path);
	std::cout << "Path with upload path: " << path << std::endl;
	if (checkFileAccess(upload_path, rooted_error_paths) == true)
	{
		std::ofstream output(path);

		if (output.is_open())
		{
			output << req.getBody();
			output.close();
			uploadFileResponse();
		}
		else
		{
			errorResponse(404, "Not Found", rooted_error_paths);
			perror("404 File creation failed");
		}
	}
}

void	Response::deleteFile(std::string path, std::map<int, std::string> error_paths){

	// if (access(path.c_str(), W_OK) == 0)
	//if (checkFileAccess(path, error_paths) == true)
	//{
		if (remove(path.c_str()) < 0)
		{
			errorResponse(500, "Internal Server Error", error_paths);
			perror("500 Delete file failed");
			return;
		}
		deleteResponse();
	//}
	// else
	// {
	// 	errorResponse(403, "Forbiddden", error_paths);
	// 	perror("403 Write access failed");
	// }
}

void Response::autoIndexResponse(std::string path, std::string dir_list, Request& req) {

	size_t j = 0;
	std::vector<std::string> vec_dir_list;

	for (size_t i = 0; i < dir_list.length(); i++) {
		if (dir_list[i] == '\n') {
			vec_dir_list.push_back(dir_list.substr(j, i - j));
			j = i + 1;
		}
	}
	vec_dir_list.erase(vec_dir_list.begin());

	this->_body = "<!DOCTYPE html>\n"
				  "<html>\n"
				  "<head>\n"
				  "<title>Index of " + req.getPathToFile() + "</title>\n"
				  "<style>\n"
				  "body {\n"
				  "    font-family: Arial, sans-serif;\n"
				  "    margin: 0;\n"
				  "    padding: 20px;\n"
				  "}\n"
				  ".file-info {\n"
				  "    display: flex;\n"
				  "    justify-content: space-between;\n"
				  "    align-items: center;\n"
				  "    width: 100%;\n"
				  "    border: 1px solid black;\n"
				  "    padding: 10px;\n"
				  "    margin-bottom: 10px;\n"
				  "    box-sizing: border-box;\n"
				  "}\n"
				  ".file-info button {\n"
				  "    flex: 0 0 auto;\n"
				  "    margin-right: 10px;\n"
				  "}\n"
				  ".file-info .creation-date {\n"
				  "    flex: 1;\n"
				  "    text-align: center;\n"
				  "}\n"
				  ".file-info .char-count {\n"
				  "    flex: 0 0 auto;\n"
				  "    text-align: right;\n"
				  "    white-space: nowrap;\n"
				  "}\n"
				  "</style>\n"
				  "</head>\n"
				  "<body>\n"
				  "<h1>Index of " + req.getPathToFile() + "</h1>\n";

	for (std::vector<std::string>::iterator it = vec_dir_list.begin(); it != vec_dir_list.end(); ++it) {
		struct stat buf;
		std::string all_path = path + "/" + *it;
		if (stat(all_path.c_str(), &buf) != 0) {
			perror("Stat failed");
			continue;
		}

		char buffer[80];
		std::string unrooted_path;
		struct tm* time_info = localtime(&buf.st_ctime);
		strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_info);

		unrooted_path = req.getPathToFile();
		if (unrooted_path.find_last_of("/") != unrooted_path.length() - 1)
			unrooted_path.insert(unrooted_path.length(), "/");

		std::string redirect_url = unrooted_path + *it;
		std::string txt_button = *it;
		std::string creation_date = buffer;
		std::string char_count = getCharCount(buf);

		insertHtmlIndexLine(redirect_url, txt_button, creation_date, char_count);
	}

	this->_body += "</body>\n</html>";
	this->_status_code = 200;
	this->_status_message = "OK";
	this->_content_type = "text/html";
	this->_content_len = this->_body.length();
	generateResponse();
}

void Response::insertHtmlIndexLine(std::string redirect_url, std::string txt_button, std::string creation_date, std::string char_count) {
	this->_body += "<div class=\"file-info\">\n"
				   "    <button onclick=\"window.location.href='" + redirect_url + "'\">" + txt_button + "</button>\n"
				   "    <div class=\"creation-date\">" + creation_date + "</div>\n"
				   "    <div class=\"char-count\">" + char_count + "</div>\n"
				   "</div>\n";
}

std::string Response::getCharCount(struct stat buf) {

	if (S_ISDIR(buf.st_mode))
		return ("-");
	else if (S_ISREG(buf.st_mode))
		return (std::to_string(buf.st_size));
	return ("Unknown");
}

void	Response::deleteResponse(void){

	this->_status_code = 200;
	this->_status_message = "OK";
	generateResponse();
}

void	Response::downloadFileResponse(std::string stack, std::map<int, std::string> error_paths){

	if (getContentType(stack).empty() == false)
	{
		this->_status_code = 200;
		this->_status_message = "OK";
		this->_content_type = getContentType(stack);
		this->_content_len = stack.length();
		this->_body = stack;
		generateResponse();
	}
	else
	{
		errorResponse(415, "Unsupported Media Type", error_paths);
		perror("415 Unsupported Media Type");
	}
}

std::string	Response::getContentType(std::string stack){

	std::stringstream	ss;
	std::stringstream	ss_hex;
	std::vector<char>	bytes(8);
	std::string			octets;
	bool				text = true;

	if (stack.compare(0, 15, "<!DOCTYPE html>") == 0)
		return ("text/html");
	for (size_t i = 0; i < stack.length(); i++)
	{
		if (!isprint(stack[i]) && !isspace(stack[i]))
		{
			std::cout << "Not a char" << std::endl;
			text = false;
			break;
		}
	}
	if (text == true)
		return ("text/plain");
	ss << stack;
	ss.read(&bytes[0], bytes.size());
	for (size_t i = 0; i < bytes.size(); i++)
		ss_hex << std::hex << (static_cast<int>(bytes[i]) & 0xFF) << " ";
	octets = ss_hex.str();
	switch (stringToEnum(octets))
	{
		case E_PNG:
			return ("image/png");
		case E_JPEG:
			return ("image/jpeg");
		case E_SVG:
			return ("image/svg+xml");
		case E_GIF:
			return ("image/gif");
		case E_PDF:
			return ("application/pdf");
		case E_ZIP:
			return ("application/zip");
		case E_MP4:
			return ("video/mp4");
		default:
			return ("");
	}
}

t_file_type	Response::stringToEnum(std::string const& str){

	if (str.compare(0, 20, "89 50 4e 47 d a 1a a") == 0) return (E_PNG);
	if (str.compare(0, 5, "ff d8") == 0) return (E_JPEG);
	if (str.compare(0, 23, "3c 3f 78 6d 6c 20 76 65") == 0) return (E_SVG);
	if (str.compare(0, 17, "47 49 46 38 39 61") == 0) return (E_GIF);
	if (str.compare(0, 11, "25 50 44 46") == 0) return (E_PDF);
	if (str.compare(0, 18, "50 4b 3 4 14 0 8 0") == 0) return (E_ZIP);
	if (str.compare(0, 20, "0 0 0 20 66 74 79 70") == 0) return (E_MP4);
	else
		return (E_UNSUP);
}

void	Response::uploadFileResponse(void){

	this->_status_code = 201;
	this->_status_message = "Created";
	generateResponse();
}

void	Response::errorResponse(int error_code, std::string message, std::map<int, std::string> error_paths){

	std::string error_path;

	std::cout << error_code << std::endl;
	error_path = matchErrorCodeWithPage(error_code, error_paths);
	if (error_path.empty() == true)
	{
		createHtmlErrorPage(error_code, message);
		generateResponse();
		return;
	}
	std::cout << "Error_path in errorResponse: " << error_path << std::endl;
	//check if the file is readable and that the content-type is accepted
	if (checkErrorFileAccess(error_path) == true)
	{
		if (checkContentType(error_path) == true)
		{
			this->_status_code = error_code;
			this->_status_message = message;
			this->_content_type = "text/html";

			std::ifstream	input(error_path);

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
				fileNotFound();
		}
		else
			errorResponse(415, "Unsupported Media Type", error_paths);
	}
}

std::string	Response::matchErrorCodeWithPage(int error_code, std::map<int, std::string> error_paths){

	for (std::map<int, std::string>::iterator it = error_paths.begin(); it != error_paths.end(); ++it)
	{
		if (it->first == error_code)
		{
			std::cout << it->first << std::endl;
			std::cout << it->second << std::endl;
			return (it->second);
		}
	}
	return ("");
}

void	Response::createHtmlErrorPage(int error_code, std::string message){

	int			i;
	int			integer[] = {301, 400, 403, 404, 405, 413, 415, 500};
	std::string	error_headers[] = {"An error occurred.", "The server could not understand the request due to invalid syntax.", "You do not have permission to access this resource on this server.", "The requested resource could not be found on this server.",
	"The method specified in the request is not allowed for the resource identified by the request URI.", "The request entity is larger than the server is willing or able to process.", "The media format of the requested data is not supported by the server.",
	"The server encountered an internal error or misconfiguration and was unable to complete your request."};

	i = 0;
	while (i < 8 && error_code != integer[i])
		i++;
	std::string header = error_headers[i];

	this->_body = "<!DOCTYPE html>\n"
				  "<html lang=\"en\">\n"
				  "<head>\n"
				  "<meta charset=\"UTF-8\">\n"
				  "<title>" + std::to_string(error_code) + " " + message + "</title>\n"
				  "</head>\n"
				  "<body>\n"
				  "<h1>" + std::to_string(error_code) + " " + message + "</h1>\n"
				  "<p>" + header + "</p>\n"
				  "</body>\n"
				  "</html>\n";
	this->_status_code = error_code;
	this->_status_message = message;
	this->_content_type = "text/html";
	this->_content_len = _body.length();
}

void	Response::fileNotFound(void){

	this->_status_code = 404;
	this->_status_message = "Not Found";
	this->_content_type = "text/plain";
	this->_body = "File not found.";
	this->_content_len = this->_body.length();
	generateResponse();
}

bool	Response::checkFileAccess(std::string path, std::map<int, std::string> error_paths){

	struct stat buf;

	if (stat(path.c_str(), &buf) != 0)
	{
		if (errno == ENOENT)
		{
			perror("404 Not Found");
			errorResponse(404, "Not Found", error_paths);
		}
		else
		{
			perror("500 Internal Server Error");
			errorResponse(500, "Internal Server Error", error_paths);
		}
		return (false);
	}
	if (access(path.c_str(), R_OK) != 0)
	{
		if (errno == EACCES)
		{
			perror("403 Forbidden");
			errorResponse(403, "Forbidden", error_paths);
		}
		else
		{
			perror("500 Internal Server Error");
			errorResponse(500, "Internal Server Error", error_paths);
		}
		return (false);
	}
	return (true);
}

bool	Response::checkRootAccess(std::string path){

	struct stat buf;

	if (stat(path.c_str(), &buf) != 0)
	{
		fileNotFound();
		return (false);
	}
	if (access(path.c_str(), R_OK) != 0)
	{
		fileNotFound();
		return (false);
	}
	return (true);
}

bool	Response::checkErrorFileAccess(std::string error_path){

	struct stat buf;

	if (stat(error_path.c_str(), &buf) != 0)
	{
		if (errno == ENOENT)
		{
			createHtmlErrorPage(404, "Not Found");
			generateResponse();
		}
		else
		{
			createHtmlErrorPage(500, "Internal Server Error");
			generateResponse();
		}
		return (false);
	}
	if (access(error_path.c_str(), R_OK) != 0)
	{
		if (errno == EACCES)
		{
			createHtmlErrorPage(403, "Forbidden");
			generateResponse();
		}
		else
		{
			createHtmlErrorPage(500, "Internal Server Error");
			generateResponse();
		}
		return (false);
	}
	return (true);
}

void	Response::generateResponse(void){

	if (this->_body.empty() == false)
	{
		this->_response = std::string("HTTP/") +\
			this->_http_version + std::string(" ") +\
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

void	Response::setVersion(std::string version){

	_http_version = version;
}

std::string	Response::getResponse(void) const{

	return _response;
}
