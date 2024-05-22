/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 11:15:20 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/22 15:29:33 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"
#include "../parser/confParser.hpp"

Router::Router(){}

Router::~Router(){}

void	Router::registerRoute(t_locations route){

	this->_routes.push_back(route);
}

void	Router::routeRequest(Request *req){

	for (std::vector<t_locations>::iterator it = _routes.begin(); it != _routes.end; ++it)
	{
		// Exact prefix location
		if (*it->location_path.(req->_path_to_file) == 0)
		{
			implementDirectives(req, *it->location_path);
			return;
		}
	}
	recursiveMatch(removeLastPath(req->path_to_file));
}

void	Router::recursiveMatch(std::string new_path_to_file){

	if (new_path_to_file.empty())
		perror("Router failed");
	// Default prefix location
	else if (new_path_to_file.compare("/") == 0)
	{
		implementDirectives(req, "/");
		return;
	}
	else
	{
		for (std::vector<t_locations>::iterator it = _routes.begin(); it != _routes.end; ++it)
		{
			// Most specific prefix location
			if (*it->location_path.(new_path_to_file) == 0)
			{
				implementDirectives(req, *it->location_path);
				return;
			}
		}
	}
	recursiveMatch(removeLastPath(new_path_to_file));
}

std::string	Router::removeLastPath(std::string path_to_file){

	int slash_position; 
	
	if (!path_to_file.empty())
	{
		slash_position = path_to_file.find_last_of('/');
		pah_to_file.erase(slash_position, path_to_file.length());
	}
	return (path_to_file);
}

void	Router::implementDirectives(std::string route){

	if (!(_routes[route].redirections.empty()))
	{
		for (std::map<std::string, std::string>::iterator it = redirections.begin(); it != redirections.end(); ++it);
		{
			// Redirections
			if (*it.compare(req->_path_to_file))
			{
				req->_path_to_file = *it;
				routeRequest();
				break;
			}
		}
	}
	if (!(_routes[route].root_path.emtpy()))
	{

	}
}
