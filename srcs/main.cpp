/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 10:35:22 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/27 16:44:59 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec/ServerManager.hpp"

typedef struct s_locations
{
	std::string							location_path;
	std::string							root_path;
	std::map<std::string, std::string>	redirections;
	std::vector<std::string>			default_path;
	std::vector<std::string>			allowed_methods;
	bool								auto_index;
}	t_locations;

typedef struct s_server_scope
{
	int									port;
	int									max_body_size;
	std::vector<std::string>			server_name;
	std::map<int, std::string>			error_page_paths;
	std::map<std::string, t_locations>	locations;
	std::string							cgi_path;
	std::string							upload_path;
}	t_server_scope;

t_server_scope	*initServer(void)
{
	t_server_scope	*server;
	t_locations		*loc;

	server->port = 8080;
	server-> error_page_paths = "var/www/html";
	loc->location_path = "/lol/caca";
	loc->root_path = "var/www/html";
	loc->default_path.push_back("index.html");
	loc->allowed_methods.push_back("GET");
	loc->auto_index = false;
	server->locations = loc;
}

int	main(int argc, char **argv)
{
	ServerManager	sm;

	(void)argc, (void)argv;
	sm.launchServer(initServer());
}
