/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 11:00:48 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/14 13:03:29 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include "Server.hpp"
# include "Client.hpp"
# include <vector>

class ServerManager
{
	public:
		ServerManager();
		~ServerManager();
		void				launchServers(void);
	
	private:
		void				createSockets(void);
		void				checkSockets(void);
		bool				checkConnection(unsigned int i) const;
		void				closeServerSockets(void) const;
		std::vector<Server> _servers;
		std::vector<Client> _clients;
};

#endif
