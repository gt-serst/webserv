/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 09:43:45 by gt-serst          #+#    #+#             */
/*   Updated: 2024/06/17 15:14:08 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "../parser/confParser.hpp"
# include <string>
# include <map>
# include <arpa/inet.h>

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
		int							getClientFd(void) const;
		t_server_scope				getConfig(void) const;
		struct sockaddr_in			getClientAddr(void) const;
		std::map<int, std::string>	getRequests(void);

	private:
		int							_fd;
		t_server_scope				_config;
		std::map<int, std::string>	_requests;
		struct sockaddr_in			_server_addr;
		struct sockaddr_in			_client_addr;
};

#endif
