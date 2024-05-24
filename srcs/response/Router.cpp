/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 11:15:20 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/24 19:13:09 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"
#include "../parser/confParser.hpp"
#include <stdio.h>
#include <string>
#include <map>

Router::Router(){}

Router::~Router(){}

t_locations	Router::routeRequest(std::string path_to_file, std::map<std::string, t_locations> routes){

	std::string	tmp;
	t_locations	location;

	tmp = path_to_file;

	location = recursiveRouteRequest(tmp, path_to_file, routes);
	if (!handleRedirection(path_to_file, it->second.redirections, routes))
		return (location);
	else

}

t_locations	Router::recursiveRouteRequest(std::string tmp, std::string path_to_file, std::map<std::string, t_locations> routes){

	if (tmp.empty() == true)
		perror("Router failed");
	for (std::map<std::string, t_locations>::iterator it = routes.begin(); it != routes.end; ++it)
	{
		if (it->first.compare(tmp) == 0)
			return (it->second);
		else
			return (nullptr);
	}
	if (tmp.compare("/") == 0)
		perror("Router failed");
	return (recursiveRouteRequest(removeLastPath(tmp), routes));
}

std::string	Router::removeLastPath(std::string tmp){

	int	slash_position;

	slash_position = tmp.find_last_of('/');
	if (slash_position == 0)
		return (std::string("/"));
	tmp.erase(slash_position, tmp.length());
	return (tmp);
}

t_locations	Router::handleRedirection(std::string path_to_file, std::map<std::string, std::string> redirections, std::map<std::string, t_locations> routes){

	std::string	tmp;
	t_locations	location;

	for (std::map<std::string, std::string>::iterator it = redirections.begin(); it != redirections.end(); ++it)
	{
		if (it->first.compare(path_to_file))
		{
			path_to_file = it->second;
			location = recursiveRouteRequest(tmp, path_to_file, redirections, routes);
			return (location);
		}
	}
	return (nullptr);
}
