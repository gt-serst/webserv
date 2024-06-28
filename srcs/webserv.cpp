/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geraudtserstevens <geraudtserstevens@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 12:16:16 by gt-serst          #+#    #+#             */
/*   Updated: 2024/06/28 16:43:03 by geraudtsers      ###   ########.fr       */
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
		char *av[] = {ft_strdup("./webserv"), ft_strdup("config/default.conf"), NULL};
		file = confChecker(2, av);
		free(av[0]);
		free(av[1]);
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
