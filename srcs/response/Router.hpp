/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 11:21:28 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/27 17:57:51 by gt-serst         ###   ########.fr       */
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
		std::string	handleRedirection(std::string path_to_file, std::map<std::string, std::string> redirections, std::map<std::string, t_locations> routes);
};

#endif
