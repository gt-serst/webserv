/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 11:15:20 by gt-serst          #+#    #+#             */
/*   Updated: 2024/06/06 11:34:43 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"
#include "../parser/confParser.hpp"
#include <stdio.h>
#include <string>
#include <map>

Router::Router(void){}

Router::~Router(void){}

bool	Router::routeRequest(std::string& path_to_file, t_locations& loc, std::map<std::string, t_locations> routes){

	std::string	tmp;

	tmp = path_to_file;
	//std::cout << "Old path: " << path_to_file << std::endl;
	loc = recursiveRouteRequest(tmp, routes);
	if (path_to_file == handleRedirection(path_to_file, loc.redirections))
	{
		//std::cout << "Path: " << path_to_file << std::endl;
		return (true);
	}
	else
	{
		std::string old_redir;
		std::string new_redir = handleRedirection(path_to_file, loc.redirections);

		while (path_to_file != new_redir)
		{
			if (old_redir == new_redir)
				return (false);
			else
			{
				path_to_file = new_redir;
				loc = recursiveRouteRequest(path_to_file, routes);
				old_redir = loc.redirections.begin()->second;
				//std::cout << old_redir << std::endl;
				new_redir = handleRedirection(path_to_file, loc.redirections);
				//std::cout << new_redir << std::endl;
			}
		}
		//std::cout << "New path after redirection: " << path_to_file << std::endl;
		return (true);
	}
}

t_locations	Router::recursiveRouteRequest(std::string tmp, std::map<std::string, t_locations> routes){

	if (tmp.empty() == true)
		perror("Router failed");
	for (std::map<std::string, t_locations>::iterator it = routes.begin(); it != routes.end(); ++it)
	{
		std::cout << "Loc: " << it->first << std::endl;
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
