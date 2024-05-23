/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 16:28:16 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/23 12:33:11 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "../parser/confParser.hpp"
#include <string>
#include <map>

Response::Response(){}

Response::~Response(){}

void	Response::handleDirective(std::string path, t_locations loc, std::map<std::string, t_locations> routes){

	if (isLocationRooted(path, loc.root_path, loc) == false)
		perror("Root failed");
	else
	{
		getFileType(path, loc, routes);
	}
}

bool	Response::isLocationRooted(std::string& path, std::string root, t_locations loc){

	if (loc.root_path.empty() == false)
		path.replace(0, loc.location_path.length(), loc.root_path);
	else
		return (false);
	return (true);
}

void	Response::getFileType(std::string path, t_locations loc, std::map<std::string, t_locations> routes){

	if (path[path.length()] == "/")
		isDir(path, loc, routes);
	else
		isFile(path, loc);
}

void	Response::isDir(std::string path, t_locations loc, std::map<std::string, t_locations> routes) {
	
	std::string	index;

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
		isMethodAllowed(path, loc);
}

void	Response::isFile(std::string path, t_locations loc){

	if (access(path, X_OK) == 0)
		isCGI(loc);
	else
		perrro("Access failed");
}
