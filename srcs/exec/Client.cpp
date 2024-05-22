/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 09:41:06 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/22 12:17:06 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(Server server) _server(server){}

Client::~Client(){}

void	Client::setRequest(Request request){

	this->_request = request;
}

Request	Client::getRequest(void) const{

	return (this->_request);
}
