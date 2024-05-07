/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:00:48 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/07 17:13:24 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include "Server.hpp"
# include <vector>
# include <poll.h>

class ServerManager
{
	private:
		ServerManager();
		~ServerManager();
		void				launchServers(void);
		void				createSockets(void);
		void				checkSockets(void);
		void				closeSockets(void);
		std::vector<Server> _servers;
		struct pollfd*		_fds;
		int					_nfds;
}

#endif
