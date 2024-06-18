#include "parser/confParser.hpp"
#include "exec/ServerManager.hpp"
#include "webserv.hpp"

int	webserv(int argc, char **argv)
{
	std::string	file = confChecker(argc, argv);
	if (file.empty())
		return (1);
	t_server_scope	*servers = NULL;
	int		serverCount = -1;
	if(!(servers = confParser(file, &serverCount)))
		return (1);
	ServerManager	sm;

	sm.launchServer(servers, serverCount + 1);
	return (0);
}
