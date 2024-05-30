/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geraudtserstevens <geraudtserstevens@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:00:48 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/30 15:56:09 by geraudtsers      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include "Server.hpp"
# include "../parser/confParser.hpp"
# include <map>
# include <vector>
# include <sys/select.h>

class ServerManager
{
	public:
		ServerManager(void);
		~ServerManager(void);
		void					launchServer(t_server_scope *servers, int nb_servers);

	private:
		void					initServer(t_server_scope *servers, int nb_servers);
		void					serverRoutine(void);
		void					clear(void);
		std::map<int, Server>	_servers;
		std::map<int, Server>	_sockets;
		std::vector<int>		_ready;
		fd_set					_fd_set;
};

#endif
