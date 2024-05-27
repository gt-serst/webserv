#include "confParser.hpp"

t_server_scope *parseServer(int *i, std::string buffer, t_server_scope *serverConfig)
{
	
}

t_server_scope *confParser(std::string buffer)
{
	t_server_scope	*serverConfig = NULL;
	for (int i = 0; buffer[i]; i++)
	{
		if (!(serverConfig = parseServer(&i, buffer, serverConfig)))
			return (NULL);
	}
	return (serverConfig);
}
