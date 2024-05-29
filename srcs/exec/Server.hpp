/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 09:43:45 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/29 13:07:39 by gt-serst         ###   ########.fr       */
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
		Server();
		Server(t_server_scope config);
		~Server();
		void						createServerSocket(Server server);
		int							listenClientConnection(void);
		int							readClientSocket(int client_fd);
		void						handleRequest(int client_fd);
		void						sendResponse(int client_fd);
		void						closeServerSocket(void) const;
		int							getFd(void) const;
		t_server_scope				getConfig(void) const;

	private:
		int							_fd;
		t_server_scope				_config;
		std::map<int, std::string>	_requests;
};

#endif
