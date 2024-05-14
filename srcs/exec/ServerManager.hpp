/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:00:48 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/14 09:51:36 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include "Server.hpp"
# include "Client.hpp"
# include <vector>
# include <poll.h>

class ServerManager
{
	public:
		ServerManager();
		~ServerManager();
		void				launchServers(void);
	
	private:
		void				createSockets(void);
		void				checkSockets(void);
		void				closeSockets(void);
		std::vector<Server> _servers;
		std::vector<Client> _clients;
		struct pollfd		_pollfds[20];
		int					nfds;
};

#endif
