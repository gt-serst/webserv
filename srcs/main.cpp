/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geraudtserstevens <geraudtserstevens@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 10:35:22 by gt-serst          #+#    #+#             */
/*   Updated: 2024/06/24 14:56:44 by febonaer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser/confParser.hpp"
#include "exec/ServerManager.hpp"
#include "exec/Server.hpp"
#include "exec/ServerManager.hpp"
#include "request/Request.hpp"
#include "response/Response.hpp"
#include "response/Router.hpp"

/*t_server_scope	*initServer(void)
{
	t_server_scope	*server = NULL;
	t_locations		loc;

	server->port = 8080;
	loc.location_path = "/lol/caca";
	loc.root_path = "var/www/html";
	loc.default_path.push_back("index.html");
	loc.allowed_methods.push_back("GET");
	loc.auto_index = false;
	server->locations.insert(std::pair<std::string, t_locations>(loc.location_path, loc));
	return (server);
}*/

int main(int argc, char **argv)
{
	return (webserv(argc, argv));
}
