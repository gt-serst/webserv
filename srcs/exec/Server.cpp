/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 09:59:24 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/22 13:06:03 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "../parser/confParser.hpp"

Server::Server(t_server_scope conf) : _conf(conf){}

Server::~Server(){}

unsigned int	Server::getPort(void) const{

	return (this->_conf.port);
}
