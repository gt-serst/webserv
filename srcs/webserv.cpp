#include "parser/confParser.hpp"
#include "exec/ServerManager.hpp"

int	webserv(int argc, char **argv)
{
	std::string	file = confChecker(argc, argv);
	if (file.empty())
		return (1);
	t_server_scope	*servers = NULL;
	int		serverCount = -1;
	if(!(servers = confParser(file, &serverCount)))
		return (1);
	std::cout << "Locations /lol root path : |" << servers[0].locations["/lol"].root_path << "|" << std::endl;
	std::cout << "Locations /lol autoindex : |" << servers[0].locations["/lol"].auto_index << "|" << std::endl;
	std::cout << "Locations /lol GET : |" << servers[0].locations["/lol"].allowed_methods["GET"] << "|" << std::endl;
	std::cout << "Locations /lol POST : |" << servers[0].locations["/lol"].allowed_methods["POST"] << "|" << std::endl;
	std::cout << "Locations /lol DELETE : |" << servers[0].locations["/lol"].allowed_methods["DELETE"] << "|" << std::endl;
	std::cout << "Locations /lol GET : |" << servers[0].locations["/lol"].allowed_methods["GET"] << "|" << std::endl;
	for (int i = 0; servers[0].locations["/lol"].default_path[i].empty() == 0; i++)
		std::cout << "Locations /lol default path " << i << " : |" << servers[0].locations["/lol"].default_path[i] << "|" << std::endl;
	ServerManager	sm;

	sm.launchServer(servers, serverCount);
	return (0);
}
