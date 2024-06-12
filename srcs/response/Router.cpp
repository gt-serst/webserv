/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 11:15:20 by gt-serst          #+#    #+#             */
/*   Updated: 2024/06/12 11:01:21 by gt-serst         ###   ########.fr       */
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

	tmp = path_to_file;
	loc = recursiveRouteRequest(tmp, routes);
	//std::cout << "Path to file: " << path_to_file << std::endl;
	//std::cout << "HandleRedirection return: " << handleRedirection(path_to_file, loc.redirections) << std::endl;
	if (path_to_file == handleRedirection(path_to_file, loc.redirections))
		resp.setRedir(false);
	else
	{
		//int			allowed_number_of_redir;
		//std::string old_redir;
		std::string new_redir = handleRedirection(path_to_file, loc.redirections);
		resp.setLocation(new_redir);
		resp.setRedir(true);

		/*allowed_number_of_redir = 0;
		while (path_to_file != new_redir)
		{
			if (allowed_number_of_redir > 10)
				return (false);
			path_to_file = new_redir;
			loc = recursiveRouteRequest(path_to_file, routes);
			new_redir = handleRedirection(path_to_file, loc.redirections);
			allowed_number_of_redir++;
		}
		resp.setLocation(new_redir);
		std::cout << "Location header in response:" << resp.getLocation() << std::endl;
		return (resp.setRedir(true), true);*/
	}
}

t_locations	Router::recursiveRouteRequest(std::string tmp, std::map<std::string, t_locations> routes){

	if (tmp.empty() == true)
		perror("Router failed");
	for (std::map<std::string, t_locations>::iterator it = routes.begin(); it != routes.end(); ++it)
	{
		if (it->first.compare(tmp) == 0)
			return (it->second);
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

std::string	Router::handleRedirection(std::string path_to_file, std::map<std::string, std::string> redirections){

	for (std::map<std::string, std::string>::iterator it = redirections.begin(); it != redirections.end(); ++it)
	{
		if (it->first.compare(path_to_file) == 0)
			return (it->second);
	}
	return (path_to_file);
}
