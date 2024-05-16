/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 09:37:41 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/16 18:23:29 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <sys/socket.h>
# include <arpa/inet.h>

class Client
{
	public:
		Client();
		Client(Client const & rhs);
		Client&	operator=(Client const & src);
		~Client();
		int					_client_fd;
		struct sockaddr_in	_client_addr;
		socklen_t			_client_addr_len;
};

#endif
