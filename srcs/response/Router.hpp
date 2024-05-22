/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 11:21:28 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/22 13:06:08 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTER_HPP
# define ROUTER_HPP
# include "../parser/confParser.hpp"

class Router{

	private:
		Router();
		~Router();
		void	registerRoute();
		void	routeRequest();
		std::vector<t_locations> _routes;
};

#endif
