/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 09:41:06 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/14 12:21:37 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(){}

Client::Client(Client const & src){

	*this = src;
	return;
}

Client &	Client::operator=(Client const & rhs){

	if (this != &rhs)
	{
		this->_client_fd = rhs._client_fd;
		this->_client_addr = rhs._client_addr;
		this->_client_addr_len = rhs._client_addr_len;
	}
	return *this;
}

Client::~Client(){}
