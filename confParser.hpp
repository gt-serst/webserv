#ifndef CONFPARSER_HPP
# define CONFPARSER_HPP

# include <string>
# include <iostream>
# include <fstream>

//----------ENUMS----------//

typedef enum e_allowed_methods
{
	GET,
	POST,
	DELETE
}	t_allowed_methods;

//----------STRUCTS----------//

typedef struct s_locations
{
	std::string	location_path; //route path (on the website)
	std::string	root_path; //path where files should be searched from
	std::string	**redirections;
	std::string	default_path; //default path if request is a directory
	std::string	*allowed_methods;
	bool		auto_index; //is directory listing allowed?
}	t_locations;

typedef struct s_server_scope
{
	int		port;
	int		max_body_size;
	std::string	server_name;
	std::string	cgi_path;
	std::string	upload_path;
	std::string	**error_page_paths;
	t_locations	*locations;
}	t_server_scope;

//----------FUNCTIONS----------//

int		webserv(int argc, char **argv);
std::string	confChecker(int argc, char **argv);
t_server_scope	*confParser(std::string buffer);

#endif
