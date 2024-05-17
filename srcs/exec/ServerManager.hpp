/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:00:48 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/17 15:36:52 by gt-serst         ###   ########.fr       */
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
		bool				serverEvent(unsigned int fd) const;
		void				listenConnections(unsigned int fd);
		std::string			readClientSocket(unsigned int fd);
		void				handleRequest(unsigned int fd, std::string buffer);
		void				sendResponse(unsigned int fd, std::string buffer);
		void				closeServerSockets(void) const;
		std::vector<Server> _servers;
		std::vector<Client> _clients;
		fd_set				_current_sockets;
		fd_set				_ready_sockets;
};

#endif
