/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 09:43:45 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/22 12:07:41 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "../parser/confParser.hpp"
# include <sys/socket.h>
# include <arpa/inet.h>

class Server
{
	public:
		Server(t_server_scope conf);
		~Server();
		unsigned int		getPort(void) const;
		int					_server_fd;
		struct sockaddr_in	_server_addr;
		socklen_t			_server_addr_len;

	private:
		t_server_scope	conf;
		
};

#endif
