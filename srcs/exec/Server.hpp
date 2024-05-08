/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 09:43:45 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/08 10:45:04 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <sys/socket.h>
# include <arpa/inet.h>

class Server
{
	public:
		Server(unsigned int port);
		~Server();
		unsigned int		getPort(void) const;
		int					_server_fd;
		struct sockaddr_in	_server_addr;
		socklen_t			_server_addr_len;

	private:
		unsigned int	_port;
		
};

#endif
