/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:00:48 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/22 13:06:05 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include "Server.hpp"
# include "Client.hpp"
# include "../parser/confParser.hpp"
# include "../response/Router.hpp"
# include <vector>

class ServerManager
{
	
	typedef s_main{
		
		Server	current_server;
		Client	current_client;
		Router	router;
	}	t_main;

	public:
		ServerManager();
		~ServerManager();
		void				launchServer(t_server_scope *servers);
	
	private:
		void				createServerSocket(void);
		void				serverRoutine(void);
		bool				serverEvent(unsigned int fd);
		void				listenClientConnection(unsigned int fd);
		std::string			readClientSocket(unsigned int fd);
		void				handleRequest(unsigned int fd, std::string buffer);
		void				sendResponse(unsigned int fd, std::string buffer);
		void				closeServerSocket(void) const;
		std::vector<Server> _servers;
		Server				_current_server;
		Client				_current_client;
		Router				_router;
		fd_set				_current_sockets;
		fd_set				_ready_sockets;
		
};

#endif
