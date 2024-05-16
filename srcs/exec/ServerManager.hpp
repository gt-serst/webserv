/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:00:48 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/16 18:23:19 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include "Server.hpp"
# include "Client.hpp"
# include <vector>

class ServerManager
{
	public:
		ServerManager();
		~ServerManager();
		void				launchServers(void);
	
	private:
		void				createSockets(void);
		void				checkSockets(void);
		bool				socketIsServer(unsigned int fd) const;
		void				listenConnections(unsigned int fd);
		int					readClientSocket(unsigned int fd);
		void				handleRequest(unsigned int fd, char* buffer);
		void				sendResponse(unsigned int fd, char* buffer);
		void				closeServerSockets(void) const;
		std::vector<Server> _servers;
		std::vector<Client> _clients;
};

#endif
