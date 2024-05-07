/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:04:51 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/07 11:43:02 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include <vector>

ServerManager::ServerManager(){}

ServerManager::~ServerManager(){}

ServerManager::launchServers(std::vector<Server> _servers){

	for (std::vector<Server>::iterator it = _servers.begin() ; it != _servers.end(); ++it)
	{

	}
}
