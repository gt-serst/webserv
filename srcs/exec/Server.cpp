/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 09:59:24 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/27 17:57:12 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "../parser/confParser.hpp"

Server::Server(){}

Server::Server(t_server_scope conf) : config(conf){}

Server::~Server(){}
