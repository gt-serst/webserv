/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 11:21:28 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/22 18:31:33 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTER_HPP
# define ROUTER_HPP

# include "../parser/confParser.hpp"
# include <map>

class Router{

	public:
		t_locations	routeRequest(std::string path_to_file, std::map<std::string, t_locations> routes);

	private:
		Router();
		~Router();
		t_location	recursiveRouteRequest(std::string tmp, std::string path_to_file, std::map<std::string, t_locations> routes);
		std::string	removeLastPath(std::string tmp);
		void		handleRedirection(std::string path_to_file, std::map<std::string, std::string> redirections, std::map<std::string, t_locations> routes);
};

#endif
