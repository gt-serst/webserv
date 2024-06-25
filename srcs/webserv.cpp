/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 12:16:16 by gt-serst          #+#    #+#             */
/*   Updated: 2024/06/25 12:16:18 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser/confParser.hpp"
#include "exec/ServerManager.hpp"
#include "webserv.hpp"

int	webserv(int argc, char **argv)
{
	std::string	file = confChecker(argc, argv);
	if (file.empty())
	{
		char *av[] = {strdup("./webserv"), strdup("config/fel.conf"), NULL};
		file = confChecker(2, av);
		if (file.empty())
			return (1);
	}
	t_server_scope	*servers = NULL;
	int		serverCount = -1;
	if(!(servers = confParser(file, &serverCount)))
		return (1);
	ServerManager	sm;

	sm.launchServer(servers, serverCount + 1);
	delete[] servers;
	return (0);
}
