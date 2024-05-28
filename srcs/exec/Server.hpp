/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geraudtserstevens <geraudtserstevens@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 09:43:45 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/27 22:27:25 by geraudtsers      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "../parser/confParser.hpp"
# include <string>
# include <sys/socket.h>
# include <arpa/inet.h>

/*typedef struct s_locations
{
	std::string							location_path;
	std::string							root_path;
	std::map<std::string, std::string>	redirections;
	std::vector<std::string>			default_path;
	std::vector<std::string>			allowed_methods;
	bool								auto_index;
}	t_locations;

typedef struct s_server_scope
{
	int									port;
	int									max_body_size;
	std::vector<std::string>			server_name;
	std::map<int, std::string>			error_page_paths;
	std::map<std::string, t_locations>	locations;
	std::string							cgi_path;
	std::string							upload_path;
}	t_server_scope;*/

class Server
{
	public:
		Server();
		Server(t_server_scope conf);
		~Server();
		t_server_scope		getConfig() const;
		int					server_fd;
		struct sockaddr_in	server_addr;
		socklen_t			server_addr_len;
	private:
		t_server_scope		_config;
};

#endif
