/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 09:43:45 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/29 15:51:39 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "../parser/confParser.hpp"
# include <string>
# include <map>

class Server
{
	public:
		Server(void);
		Server(t_server_scope config);
		~Server(void);
		int							createServerSocket(Server server);
		int							listenClientConnection(void);
		int							readClientSocket(int client_fd);
		void						handleRequest(int client_fd);
		void						sendResponse(int client_fd);
		void						closeServerSocket(void);
		void						closeClientSocket(int client_fd);
		int							getFd(void) const;
		t_server_scope				getConfig(void) const;

	private:
		int							_fd;
		t_server_scope				_config;
		std::map<int, std::string>	_requests;
};

#endif
