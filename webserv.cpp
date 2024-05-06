#include "confParser.hpp"

int	webserv(int argc, char **argv)
{
	std::string	file = confChecker(argc, argv);
	if (file.empty())
		return (1);
	t_server_scope	*servers = NULL;
	if(!(servers = confParser(file)))
		return (1);
	return (0);
}
