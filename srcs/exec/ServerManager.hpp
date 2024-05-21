/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:00:48 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/21 12:58:19 by gt-serst         ###   ########.fr       */
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
		void				launchServer(void);
	
	private:
		void				createServerSocket(void);
		void				serverRoutine(void);
		bool				serverEvent(unsigned int fd) const;
		void				listenClientConnection(unsigned int fd);
		std::string			readClientSocket(unsigned int fd);
		void				handleRequest(unsigned int fd, std::string buffer);
		void				sendResponse(unsigned int fd, std::string buffer);
		void				closeServerSocket(void) const;
		std::vector<Server> _servers;
		std::vector<Client> _clients;
		fd_set				_current_sockets;
		fd_set				_ready_sockets;
};

#endif
