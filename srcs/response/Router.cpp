/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geraudtserstevens <geraudtserstevens@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 11:15:20 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/30 23:10:12 by geraudtsers      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"
#include "../parser/confParser.hpp"
#include <stdio.h>
#include <string>
#include <map>

Router::Router(void){}

Router::~Router(void){}

t_locations	Router::routeRequest(std::string& path_to_file, std::map<std::string, t_locations> routes){

	std::string	tmp;
	t_locations	location;

	tmp = path_to_file;
	std::cout << "Old path: " << path_to_file << std::endl;
	location = recursiveRouteRequest(tmp, routes);
	if (path_to_file == handleRedirection(path_to_file, location.redirections))
	{
		std::cout << "Path: " << path_to_file << std::endl;
		return (location);
	}
	else
	{
		std::string redirect_path = handleRedirection(path_to_file, location.redirections);
		while (path_to_file != redirect_path)
		{
			path_to_file = redirect_path;
			location = recursiveRouteRequest(path_to_file, routes);
			redirect_path = handleRedirection(path_to_file, location.redirections);
		}
		std::cout << "New path after redirection: " << path_to_file << std::endl;
		return (location);
	}
}

t_locations	Router::recursiveRouteRequest(std::string tmp, std::map<std::string, t_locations> routes){

	if (tmp.empty() == true)
		perror("Router failed");
	for (std::map<std::string, t_locations>::iterator it = routes.begin(); it != routes.end(); ++it)
	{
		//std::cout << "Loc: " << it->first << std::endl;
		//std::cout << "Tmp path: " << tmp << std::endl;
		if (it->first.compare(tmp) == 0)
			return (it->second);
	}
	//std::cout << "No match" << std::endl;
	if (tmp.compare("/") == 0)
		perror("Router failed");
	return (recursiveRouteRequest(removeLastPath(tmp), routes));
}

std::string	Router::removeLastPath(std::string tmp){

	int	slash_position;

	//std::cout << "Remove Last Path" << std::endl;
	slash_position = tmp.find_last_of('/');
	if (slash_position == 0)
		return (std::string("/"));
	tmp.erase(slash_position, tmp.length());
	//std::cout << "New path: " << tmp << std::endl;
	return (tmp);
}

std::string	Router::handleRedirection(std::string path_to_file, std::map<std::string, std::string> redirections){

	//std::cout << "Searching for redirections" << std::endl;
	for (std::map<std::string, std::string>::iterator it = redirections.begin(); it != redirections.end(); ++it)
	{
		//std::cout << "Redirection: " << it->first << std::endl;
		//std::cout << "Path: " << path_to_file << std::endl;
		if (it->first.compare(path_to_file) == 0)
		{
			//std::cout << "Find redirection" << std::endl;
			//std::cout << "New redirect path: " << it->second << std::endl;
			return (it->second);
		}
	}
	return (path_to_file);
}
