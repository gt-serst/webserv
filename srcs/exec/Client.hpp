/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 09:37:41 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/22 18:27:46 by gt-serst         ###   ########.fr       */
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
		~Client();
		int					client_fd;
		struct sockaddr_in	client_addr;
		socklen_t			client_addr_len;
};

#endif
