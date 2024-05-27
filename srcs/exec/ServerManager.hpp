/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:00:48 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/27 16:57:46 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include "Server.hpp"
# include "Client.hpp"
//# include "../parser/confParser.hpp"
# include "../request/Request.hpp"
# include "../response/Router.hpp"
# include "../response/Response.hpp"
# include <string>
# include <vector>

class ServerManager
{
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
		Request				_current_request;
		Response			_current_response;
		Router				_router;
		fd_set				_current_sockets;
		fd_set				_ready_sockets;

};

#endif
