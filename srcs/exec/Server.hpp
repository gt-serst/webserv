/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geraudtserstevens <geraudtserstevens@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 09:43:45 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/30 15:42:26 by geraudtsers      ###   ########.fr       */
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
		int							createServerSocket(void);
		int							listenClientConnection(void);
		int							readClientSocket(int client_fd);
		int							handleRequest(int client_fd);
		int							sendResponse(int client_fd);
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
