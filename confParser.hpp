#ifndef CONFPARSER_HPP
# define CONFPARSER_HPP

# include <string>
# include <iostream>
# include <fstream>
# include <cctype>
# include <vector>
# include <map>

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
	std::string				root_path; //path where files should be searched from
	std::map<std::string, std::string>	redirections;
	std::vector<std::string>		default_path; //default path if request is a directory
	std::map<std::string, bool>		allowed_methods;
	bool					auto_index; //is directory listing allowed?
}	t_locations;

typedef struct s_server_scope
{
	int					port;
	int					max_body_size;
	std::vector<std::string>		server_name;
	std::map<int, std::string>		error_page_paths;
	std::map<std::string, t_locations>	locations;
	std::string				cgi_path;
	std::string				upload_path;
}	t_server_scope;

//----------FUNCTIONS----------//

int						webserv(int argc, char **argv);
std::string					confChecker(int argc, char **argv);
t_server_scope					*confParser(std::string buffer);

#endif
