/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 11:15:20 by gt-serst          #+#    #+#             */
/*   Updated: 2024/06/17 12:03:30 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"
#include "../parser/confParser.hpp"
#include "../response/Response.hpp"
#include <stdio.h>
#include <string>
#include <map>

Router::Router(void){}

Router::~Router(void){}

void	Router::routeRequest(std::string& path_to_file, t_locations& loc, std::map<std::string, t_locations> routes, Response& resp){

	std::string	tmp;

	if (path_to_file[0] != '/')
		path_to_file.insert(0, "/");
	tmp = path_to_file;
	loc = recursiveRouteRequest(tmp, routes);
	std::cout << "HandleRedirection return: " << handleRedirection(path_to_file, loc.redirections) << std::endl;
	// No redir is found
	if (path_to_file == handleRedirection(path_to_file, loc.redirections))
		resp.setRedir(false);
	else
	{
		// Redir is found, keep the new url in memory, we will use it in the redirect response
		std::string new_redir = handleRedirection(path_to_file, loc.redirections);
		resp.setLocation(new_redir);
		resp.setRedir(true);
	}
}

t_locations	Router::recursiveRouteRequest(std::string tmp, std::map<std::string, t_locations> routes){

	// Each loop recall the same function but each step a part fo the path is cutted until the default path /
	if (tmp.empty() == true)
		perror("Router failed");
	for (std::map<std::string, t_locations>::iterator it = routes.begin(); it != routes.end(); ++it)
	{
		// Comparison between the cutted path and each location in the config file
		if (it->first.compare(tmp) == 0)
			return (it->second);
	}
	// If code arrive here and do not find the default path / in the loop before an error occurs
	if (tmp.compare("/") == 0)
		perror("Router failed");
	return (recursiveRouteRequest(removeLastPath(tmp), routes));
}

std::string	Router::removeLastPath(std::string tmp){

	int	slash_position;

	// Get the last slash position and remove this part of the path
	slash_position = tmp.find_last_of('/');
	if (slash_position == 0)
		return (std::string("/"));
	tmp.erase(slash_position, tmp.length());
	return (tmp);
}

std::string	Router::handleRedirection(std::string path_to_file, std::map<std::string, std::string> redirections){

	for (std::map<std::string, std::string>::iterator it = redirections.begin(); it != redirections.end(); ++it)
	{
		// Comparisson between the path and each url from redirections in the config file
		if (it->first.compare(path_to_file) == 0)
			return (it->second);
	}
	// If no match is found, return the original path
	return (path_to_file);
}
