/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 16:28:16 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/23 17:12:21 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "../parser/confParser.hpp"
#include "../request/Request.hpp"
#include <string>
#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <dirent.h>

Response::Response(){}

Response::~Response(){}

void	Response::handleDirective(std::string path, t_locations loc, std::map<std::string, t_locations> routes, Request *req){

	if (isLocationRooted(path, loc.root_path, loc) == false)
		perror("Root failed");
	else
		getFileType(path, loc, routes, req);
}

bool	Response::isLocationRooted(std::string& path, std::string root, t_locations loc){

	if (loc.root_path.empty() == false)
		path.replace(0, loc.location_path.length(), loc.root_path);
	else
		return (false);
	return (true);
}

void	Response::getFileType(std::string path, t_locations loc, std::map<std::string, t_locations> routes, Request *req){

	struct stat path_stat;

	if (stat(path.c_str(), &path_stat) != 0)
		perror("Stat failed");
	if (S_ISDIR(path_stat.st_mode) == true)
		isDir(path, loc, routes, req);
	else if (S_ISREG(path_stat.st_mode) == true)
		isFile(path, loc, req);
	else
		perror("Neither a dir nor a file");
}

void	Response::isDir(std::string path, t_locations loc, std::map<std::string, t_locations> routes, Request *req) {
	
	std::string	index;

	if (path[path.length()] == "/")
	{	
		if(loc.default_path.empty() == false)
		{
			for (int i = 0; i < loc.default_path.size(); i++)
			{
				index = path;
				index.append(loc.default_path[i]);
				if (access(index, X_OK) == 0)
					routeRequest(loc.default_path[i], routes);
			}
			perror("Access failed");
		}
		else
			if (isMethodAllowed(loc, req) == true)
				runDirMethod(path, loc, req)
			else
				perror("Method failed");
	}
	else
	{
		if (req->_request_method == DELETE)
			perror("301 URI failed");
		if (req->_request_method == GET)
			perror("409 URI failed");
		else
			perror("URI failed");
	}
}

void	Response::isFile(std::string path, t_locations loc, Request *req){

	if (access(path, X_OK) == 0)
	{
		if (isCGI(path, loc, req) == true && isMethodAllowed(loc, req) == true)
			runCGI(cgi_path);
		else if (isMethodAllowed(loc, req) == true)
			runFileMethod(path, loc, req);
		else
			perror("Method failed");
	else
		perror("Access failed");
}

bool	Response::isMethodAllowed(t_locations loc, Request *req){

	for (int i = 0; i < loc.allowed_methods.size(); i++)
	{
		if (req->_request_method == loc.allowed_methods[i])
			return (true);
	}
	return (false);
}

void	Response::runDirMethod(std::string path, t_locations loc, Request *req){

	DIR *dr;

	if ((dr = opendir(path)) != NULL)
	{
		if (req->_request_method == GET)
			isAutoIndex(dr, path, loc);
		else if (req->_request_method == DELETE)
			deleteDir(dr, path);
	}
	else
		perror("Opendir failed");
}

void	Response::isAutoIndex(DIR *dr, std::string path, t_locations loc){

	struct dirent	*de;
	std::string		dir_list;

	if (loc.autoindex == true)
	{
		while ((de = readdir(dr)) != NULL)
			dir_list.append(de->d_name);
		this->_response = dir_list;
		closedir(dr);
	}
	else
	{
		closedir(dr);
		perror("403 Autoindex failed");
	}
}

void	Response::deleteDir(DIR *dr, std::string path){
	
	if (access(path, W_OK) == 0)
	{
		if (rmdir(path) < 0)
			perror("500 Delete directory failed");
	}
	else
	{
		closedir(dr);
		perror("403 Write access failed");
	}
}

void	Response::isCGI(std::string path, t_locations loc, Request *req){

	if (loc.cgi_path.empty() == false)
		return (true);
	else if (loc.cgi_path.empty() == true)
		return (false);
}

void	Response::runFileMethod(std::string path, t_locations loc, Request *req){
	
	if (req->_request_method == GET)
		openFile(path);
	else if (req->_request_method == POST)
		uploadFile(path, req);
	else if (req->_request_method == DELETE)
		deleteFile(path);
}

void	Response::openFile(std::string path){
	
	std::ifstream input(path);

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
		getResponse(path);
	}
	else
		perror("Open failed");
}

void	Response::uploadFile(std::string path, Request *req){
	
	if (access(path, W_OK) == 0)
	{
		std::ofstream output(path);

		if (output.is_open())
		{
			output << req->_body;
			output.close();
			postResponse(path, req);
		}
		else
			perror("File creation failed");
	}
	else
		perror("Write access failed");
}

void	Response::deleteFile(std::string path){

	if (remove(path) < 0)
		perror("Delete file failed");
	deleteResponse(path);
}
