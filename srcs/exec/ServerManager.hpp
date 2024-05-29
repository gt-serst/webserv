/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:00:48 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/29 13:07:35 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include "Server.hpp"
# include "../parser/confParser.hpp"
# include <map>
# include <vector>

class ServerManager
{
	public:
		ServerManager();
		~ServerManager();
		void					launchServer(t_server_scope *servers, int nb_servers);

	private:
		void					serverRoutine(void);
		std::map<int, Server>	_servers;
		std::map<int, Server>	_sockets;
		std::vector<int>		_ready;
};

#endif
