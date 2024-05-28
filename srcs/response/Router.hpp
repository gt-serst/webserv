/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geraudtserstevens <geraudtserstevens@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 11:21:28 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/27 23:48:59 by geraudtsers      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTER_HPP
# define ROUTER_HPP

#include "../parser/confParser.hpp"
# include <string>
# include <map>
# include <vector>

class Router{

	public:
		Router();
		~Router();
		t_locations	routeRequest(std::string& path_to_file, std::map<std::string, t_locations> routes);

	private:
		t_locations	recursiveRouteRequest(std::string tmp, std::map<std::string, t_locations> routes);
		std::string	removeLastPath(std::string tmp);
		std::string	handleRedirection(std::string path_to_file, std::map<std::string, std::string> redirections);
};

#endif
