/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 12:27:45 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/27 14:43:20 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"
#include "string"
#include "map"
#include "vector"
#include <iostream>

int	main(int argc, char **argv)
{
	Router	router;
	std::string path_to_file;
	t_locations return_loc;
	t_locations loc1;
	t_locations loc2;
	t_locations loc3;
	t_locations loc4;
	t_locations loc5;
	t_locations loc6;
	std::map<std::string, t_locations> routes;

	loc1.location_path = "/lol/caca";
	loc1.redirections.insert(std::pair<std::string, std::string>("/lol/caca/pipi", "/lol/caca/prout"));
	loc2.location_path = "/lol/caca/prout";
	loc2.redirections.insert(std::pair<std::string, std::string>("/lol/caca/prout", "/lol"));
	loc3.location_path = "/lol";
	loc3.redirections.insert(std::pair<std::string, std::string>("/lol", "/lol/troll"));
	loc4.location_path = "/lol/troll";
	loc4.redirections.insert(std::pair<std::string, std::string>("/lol/troll", "/orc"));
	loc5.location_path = "/orc";
	loc5.redirections.insert(std::pair<std::string, std::string>("/orc", "/"));
	loc6.location_path = "/";
	loc6.redirections.insert(std::pair<std::string, std::string>("/", "/lol/caca"));
	path_to_file = "/lol/caca/pipi";

	routes.insert(std::pair<std::string, t_locations>(loc1.location_path, loc1));
	routes.insert(std::pair<std::string, t_locations>(loc2.location_path, loc2));
	routes.insert(std::pair<std::string, t_locations>(loc3.location_path, loc3));
	routes.insert(std::pair<std::string, t_locations>(loc4.location_path, loc4));
	routes.insert(std::pair<std::string, t_locations>(loc5.location_path, loc5));
	routes.insert(std::pair<std::string, t_locations>(loc6.location_path, loc6));
	return_loc = router.routeRequest(path_to_file, routes);
	std::cout << path_to_file << std::endl;
	std::cout << return_loc.location_path << std::endl;
}
