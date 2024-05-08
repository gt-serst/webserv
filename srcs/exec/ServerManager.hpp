/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:00:48 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/08 13:16:20 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include "Server.hpp"
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
		void				setBlocking(int fd, bool val);
		std::vector<Server> _servers;
		struct pollfd		_fds[254];
		int					_nfds;
};

#endif
