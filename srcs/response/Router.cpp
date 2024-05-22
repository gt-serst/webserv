/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 11:15:20 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/22 13:05:58 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"
#include "../parser/confParser.hpp"

Router::Router(){}

Router::~Router(){}

void	Router::registerRoute(t_locations route){

	this->_routes.push_back(route);
}

void	Router::routeRequest(){}
