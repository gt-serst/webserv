#include "confParser.hpp"

static void		freeConfig(t_server_scope *serverConfig, int servers)
{
	(void)servers;
	/*for (unsigned int i = 0; i <= *servers; i++)
	  {
	  std::map<std::string, t_locations>::iterator it = serverConfig[*servers].locations;
	  while (it != serverConfig[*servers].locations.end())
	  {
	  if (it->second.root_path.empty() == 0)
	  delete it->second.root_path;
	  ++it;
	  }
	  }*/
	delete[] serverConfig;
}

static bool		isServer(int i, std::string buffer)
{
	if (buffer.substr(i, 6) != "server" || buffer[i + 6] != '\n' || buffer[i + 7] != '{' || buffer[i + 8] != '\n')
	{
		std::cerr << "ERROR: Wrong syntax at 'server' line." << std::endl;
		return (false);
	}
	return (true);
}

/*static t_server_scope	*isServerName(int *i, std::string buffer, t_server_scope *serverConfig, int *servers)
  {
  if (buffer.substr(*i, 12) == "server_name\t")
  {
  int count = 0;
 *i += 12;
 while (buffer[*i] && buffer[*i] != '\n')
 {
 int	j = *i;
 while (buffer[j] && isprint(buffer[j]) && buffer[j] != ' ')
 j++;
 if (j != *i && buffer[j] && (buffer[j] == '\n' || buffer[j] == ' '))
 {
 count++;
 std::string *temp = NULL;
 temp = new std::string[count];
 if (count > 1)
 {
 for (int k = 0; k < count; k++)
 temp[k] = serverConfig[*servers].server_name[k];
 delete[] serverConfig[*servers].server_name;
 }
 temp[count - 1] = buffer.substr(*i, j - *i);
 serverConfig[*servers].server_name = temp;
 *i = j + 1;
 std::cout << "server_name : " << serverConfig[*servers].server_name[count - 1] << std::endl;
//return (serverConfig);
}
else
{
freeConfig(serverConfig, *servers);
return (NULL);
}
}
return (serverConfig);
}
freeConfig(serverConfig, *servers);
return (NULL);
}*/

static t_server_scope *isServerName(int *i, std::string buffer, t_server_scope *serverConfig, int *servers)
{
	if (buffer.substr(*i, 13) != "server_name\t\t")
	{
		freeConfig(serverConfig, *servers);
		return (NULL);
	}
	*i += 12;
	std::vector<std::string> names;
	while (buffer[*i] && buffer[*i] != '\n')
	{
		*i += 1;
		int j = *i;
		while (buffer[j] && isprint(buffer[j]) && buffer[j] != ' ')
			j++;
		if (j != *i && buffer[j] && (buffer[j] == '\n' || buffer[j] == ' '))
		{
			names.push_back(buffer.substr(*i, j - *i));
			*i = j;
		}
		else
		{
			freeConfig(serverConfig, *servers);
			return (NULL);
		}
	}
	*i += 1;
	serverConfig[*servers].server_name = names;
	return (serverConfig);
}

static t_server_scope *isServerErrPage(int *i, std::string buffer, t_server_scope *serverConfig, int *servers)
{
	if (buffer.substr(*i, 12) != "error_page\t\t")
	{
		freeConfig(serverConfig, *servers);
		return (NULL);
	}
	*i += 11;
	std::map<int, std::string> result;
	while (buffer[*i] && buffer[*i] != '\n')
	{
		*i += 1;
		int j = *i;
		while (buffer[j] && isdigit(buffer[j]))
			j++;
		int error_code = stoi(buffer.substr(*i, j - *i));
		if (j != *i && buffer[j] && buffer[j] == ' ' && error_code >= 400 && error_code < 600)
		{
			j++;
			*i = j;
			while (buffer[j] && isprint(buffer[j]) && buffer[j] != ' ')
				j++;
			if (buffer[j] && (buffer[j] == ' ' || buffer[j] == '\n'))
			{
				result[error_code] = buffer.substr(*i, j - *i);
				*i = j;
			}
			else
			{
				freeConfig(serverConfig, *servers);
				return (NULL);
			}
		}
		else
		{
			freeConfig(serverConfig, *servers);
			return (NULL);
		}
	}
	*i += 1;
	serverConfig[*servers].error_page_paths = result;
	return (serverConfig);
}

static t_server_scope	*isServerHost(int *i, std::string buffer, t_server_scope *serverConfig, int *servers)
{
	if (buffer.substr(*i, 9) == "listen\t\t\t")
	{
		*i += 9;
		int j =	*i;
		while (isdigit(buffer[j]))
			j++;
		if (j == *i)
		{
			freeConfig(serverConfig, *servers);
			return (NULL);
		}
		int res = stoi(buffer.substr(*i, j - *i));
		*i = j;
		if (buffer[*i] != '\n' || res < 0 || res > 65353)
		{
			freeConfig(serverConfig, *servers);
			return (NULL);
		}
		serverConfig[*servers].port = res;
		*i += 1;
		return (serverConfig);
	}
	freeConfig(serverConfig, *servers);
	return (NULL);
}

static t_server_scope *isServerUploadPath(int *i, std::string buffer, t_server_scope *serverConfig, int *servers)
{
	if (buffer.substr(*i, 13) != "upload_path\t\t")
	{
		freeConfig(serverConfig, *servers);
		return (NULL);
	}
	*i += 13;
	int j = *i;
	while (buffer[j] && buffer[j] != '\n' && buffer[j] != ' ' && isprint(buffer[j]))
		j++;
	if (j != *i && buffer[j] && buffer[j] == '\n')
	{
		serverConfig[*servers].upload_path = buffer.substr(*i, j - *i);
		*i = j + 1;
		return (serverConfig);
	}
	freeConfig(serverConfig, *servers);
	return (NULL);
}

static t_server_scope	*isServerCGI(int *i, std::string buffer, t_server_scope *serverConfig, int *servers)
{
	if (buffer.substr(*i, 6) == "CGI\t\t\t")
	{
		*i += 6;
		int j = *i;
		while (buffer[j] && isprint(buffer[j]) && buffer[j] != ' ')
			j++;
		if (j != *i)
		{
			std::map<std::string, std::string> result;
			if (serverConfig[*servers].cgi_path.empty() == 0)
				result = serverConfig[*servers].cgi_path;
			std::string path = buffer.substr(*i, j - *i);
			j += 1;
			*i = j;
			if (buffer[j] == '.')
			{
				while (buffer[j] && isprint(buffer[j]) && buffer[j] != ' ')
					j++;
			}
			if (j != *i && buffer[j] == '\n')
			{
				result[buffer.substr(*i, j - *i)] = path;
				*i = j + 1;
				serverConfig[*servers].cgi_path = result;
				return (serverConfig);
			}
		}
	}
	freeConfig(serverConfig, *servers);
	return (NULL);
}

/*static t_server_scope *isServerCGI(int *i, std::string buffer, t_server_scope *serverConfig, int *servers)
  {
  if (buffer.substr(*i, 6) != "CGI\t\t\t")
  {
  freeConfig(serverConfig, *servers);
  return (NULL);
  }
 *i += 6;
 int j = *i;
 while (buffer[j] && buffer[j] != '\n' && buffer[j] != ' ' && isprint(buffer[j]))
 j++;
 if (j != *i && buffer[j] && buffer[j] == '\n')
 {
 serverConfig[*servers].cgi_path = buffer.substr(*i, j - *i);
 *i = j + 1;
 return (serverConfig);
 }
 freeConfig(serverConfig, *servers);
 return (NULL);
 }*/

static t_server_scope	*isServerMaxBodySize(int *i, std::string buffer, t_server_scope *serverConfig, int *servers)
{
	if (buffer.substr(*i, 21) == "client_max_body_size\t")
	{
		*i += 21;
		int j =	*i;
		while (isdigit(buffer[j]))
			j++;
		if (j == *i)
		{
			freeConfig(serverConfig, *servers);
			return (NULL);
		}
		int res = stoi(buffer.substr(*i, j - *i));
		*i = j;
		if (buffer[*i] != '\n' || res < 1 || res > 1000000)
		{
			freeConfig(serverConfig, *servers);
			return (NULL);
		}
		serverConfig[*servers].max_body_size = res;
		*i += 1;
		return (serverConfig);
	}
	freeConfig(serverConfig, *servers);
	return (NULL);
}

static t_locations	*handleA(int *i, std::string buffer, t_locations *res)
{
	if (buffer.substr(*i, 10) == "autoindex ")
	{
		*i += 10;
		if (buffer.substr(*i, 3) == "on\n")
		{
			*i += 3;
			res->auto_index = true;
			return (res);
		}
		else if (buffer.substr(*i, 4) == "off\n")
		{
			*i += 4;
			res->auto_index = false;
			return (res);
		}
		return (nullptr);
	}
	else if (buffer.substr(*i, 15) == "allowed_methods")
	{
		std::map<std::string, bool> result;
		result["GET"] = false;
		result["POST"] = false;
		result["DELETE"] = false;
		*i += 15;
		while (buffer[*i] && buffer[*i] == ' ')
		{
			if (buffer.substr(*i, 4) == " GET")
			{
				*i += 4;
				result["GET"] = true;
			}
			else if (buffer.substr(*i, 5) == " POST")
			{
				*i += 5;
				result["POST"] = true;
			}
			else if (buffer.substr(*i, 7) == " DELETE")
			{
				*i += 7;
				result["DELETE"] = true;
			}
		}
		if (buffer[*i] == 0 || buffer[*i] != '\n' || (result["GET"] == false && result["POST"] == false && result["DELETE"] == false))
			return (nullptr);
		res->allowed_methods = result;
		return (res);
	}
	return (nullptr);
}

static t_locations	*handleR(int *i, std::string buffer, t_locations *res)
{
	if (buffer.substr(*i, 5) == "root ")
	{
		*i += 5;
		int j = *i;
		while (buffer[j] && buffer[j] != '\n' && buffer[j] != ' ' && isprint(buffer[j]))
			j++;
		if (j != *i && buffer[j] && buffer[j] == '\n')
		{
			res->root_path = buffer.substr(*i, j - *i);
			*i = j + 1;
			return (res);
		}
		return (nullptr);
	}
	else if (buffer.substr(*i, 12) == "redirections" && buffer[*i + 12] == ' ')
	{
		*i += 12;
		std::map<std::string, std::string> result;
		while (buffer[*i] && buffer[*i] == ' ')
		{
			*i += 1;
			int	j = *i;
			while (buffer[j] && buffer[j] != '\n' && buffer[j] != ' ' && isprint(buffer[j]))
				j++;
			std::string	tmp = buffer.substr(*i, j - *i);
			if (j != *i && buffer[j] && buffer[j] == ' ' && buffer[j] != '\n')
			{
				j++;
				*i = j;
				while (buffer[j] && isprint(buffer[j]) && buffer[j] != ' ')
					j++;
				if (buffer[j] && (buffer[j] == ' ' || buffer[j] == '\n'))
				{
					result[tmp] = buffer.substr(*i, j - *i);
					*i = j;
				}
				else
					return (nullptr);
			}
			else
				return (nullptr);
		}
		if (buffer[*i] && buffer[*i] == '\n')
		{
			res->redirections = result;
			return (res);
		}
		return (nullptr);
	}
	return (nullptr);
}

static t_locations	*handleD(int *i, std::string buffer, t_locations *res)
{
	if (buffer.substr(*i, 7) == "default" && buffer[*i + 7] == ' ')
	{
		*i += 7;
		std::vector<std::string> indexes;
		while (buffer[*i] && buffer[*i] != '\n')
		{
			*i += 1;
			int	j = *i;
			while (buffer[j] && isprint(buffer[j]) && buffer[j] != ' ')
				j++;
			if (j != *i && buffer[j] && (buffer[j] == '\n' || buffer[j] == ' '))
			{
				indexes.insert(indexes.begin(), buffer.substr(*i, j - *i));
				*i = j;
			}
			else
				return (nullptr);
		}
		if (buffer[*i] && buffer[*i] == '\n')
		{
			(*i)++;
			res->default_path = indexes;
			return (res);
		}
	}
	return (nullptr);
}

static t_locations	*initLocation()
{
	t_locations *res = new t_locations;
	if (!res)
		return (nullptr);
	res->root_path = "";
	res->auto_index = false;
	return (res);
}

static t_locations	*getLocationParams(int *i, std::string buffer)
{
	t_locations *res = initLocation();
	if (!res)
		return (nullptr);
	while (buffer[*i] && buffer.substr(*i, 2) != "\t}")
	{
		if (buffer[*i] && buffer.substr(*i, 4) != "\t\t\t\t" && buffer[*i] != '\n')
			return (nullptr);
		if (buffer[*i] == '\n')
		{
			(*i)++;
			continue ;
		}
		*i += 4;
		switch (buffer[*i])
		{
			case 'a' :
				if (!(res = handleA(i, buffer, res)))
					return (nullptr);
				break ;
			case 'r' :
				if (!(res = handleR(i, buffer, res)))
					return (nullptr);
				break ;
			case 'd' :
				if (!(res = handleD(i, buffer, res)))
					return (nullptr);
				break ;
			default :
				return (nullptr);
		}
	}
	return (res);
}

static t_server_scope	*isServerLocation(int *i, std::string buffer, t_server_scope *serverConfig, int *servers, int *locs)
{
	if (buffer.substr(*i, 9) == "Location ")
	{
		*i += 9;
		int j = *i;
		while (buffer[j] && isprint(buffer[j]) && buffer[j] != ' ')
			j++;
		if (j != *i && buffer.substr(j, 4) == "\n\t{\n")
		{
			std::map<std::string, t_locations> result;
			if (*locs > 0)
				result = serverConfig[*servers].locations;
			std::string location = buffer.substr(*i, j - *i);
			*i = j + 4;
			t_locations	*res = nullptr;
			if (!(res = getLocationParams(i, buffer)))
			{
				freeConfig(serverConfig, *servers);
				return (NULL);
			}
			result[location] = *res;
			delete res;
			if (buffer.substr(*i, 2) == "\t}")
			{
				*i += 2;
				serverConfig[*servers].locations = result;
				(*locs)++;
				return (serverConfig);
			}
		}
	}
	freeConfig(serverConfig, *servers);
	return (NULL);
}

static t_server_scope	*getServerConfig(int *i, std::string buffer, t_server_scope *serverConfig, int *servers)
{
	int locs = 0;
	//While inside a 'server' block, calling the appropriate function according to the first character.
	while (buffer[*i] && buffer[*i] != '}')
	{
		if (buffer[*i] && buffer[*i] != '\t' && buffer[*i] != '\n')
		{
			freeConfig(serverConfig, *servers);
			return (NULL);
		}
		if (buffer[*i] && buffer[*i] == '\n')
		{
			*i += 1;
			continue ;
		}
		*i += 1;
		switch (buffer[*i])
		{
			case 's' :
				if (!(serverConfig = isServerName(i, buffer, serverConfig, servers)))
				{
					freeConfig(serverConfig, *servers);
					return (NULL);
				}
				break ;
			case 'l' :
				if (!(serverConfig = isServerHost(i, buffer, serverConfig, servers)))
				{
					freeConfig(serverConfig, *servers);
					return (NULL);
				}
				break ;
			case 'e' :
				if (!(serverConfig = isServerErrPage(i, buffer, serverConfig, servers)))
				{
					freeConfig(serverConfig, *servers);
					return (NULL);
				}
				break ;
			case 'c' :
				if (!(serverConfig = isServerMaxBodySize(i, buffer, serverConfig, servers)))
				{
					freeConfig(serverConfig, *servers);
					return (NULL);
				}
				break ;
			case 'C' :
				if (!(serverConfig = isServerCGI(i, buffer, serverConfig, servers)))
				{
					freeConfig(serverConfig, *servers);
					return (NULL);
				}
				break ;
			case 'u' :
				if (!(serverConfig = isServerUploadPath(i, buffer, serverConfig, servers)))
				{
					freeConfig(serverConfig, *servers);
					return (NULL);
				}
				break ;
			case 'L' :
				if (!(serverConfig = isServerLocation(i, buffer, serverConfig, servers, &locs)))
				{
					freeConfig(serverConfig, *servers);
					return (NULL);
				}
				break ;
			case '\n' :
				*i += 1;
				break ;
			default :
				freeConfig(serverConfig, *servers);
				return (NULL);
		}
	}
	if (buffer[*i + 1] && buffer[*i + 1] != '\n')
	{
		freeConfig(serverConfig, *servers);
		return (NULL);
	}
	*i += 1;
	return (serverConfig);
}

static t_server_scope	*parseServer(int *i, std::string buffer, t_server_scope *serverConfig, int *servers)
{
	//Checking for "server" line, passing and reallocating it if it's a new server.
	if (!isServer(*i, buffer))
	{
		freeConfig(serverConfig, *servers);
		return (NULL);
	}
	*servers += 1;
	*i += 9;
	t_server_scope *temp = new t_server_scope[*servers + 1];
	if (*servers > 0) {
		for (int j = 0; j < *servers; ++j) {
			temp[j] = serverConfig[j];
		}
		delete[] serverConfig;
	}
	serverConfig = temp;
	//Checking for each line and calling the appropriate function while we're in the server block.
	if (!(serverConfig = getServerConfig(i, buffer, serverConfig, servers)))
		return (NULL);
	return (serverConfig);
}

static t_server_scope	*checkConfig(t_server_scope *serverConfig, int *servers)
{
	//Loop over every server, loop over every location to check and set defaults values
	//Atleast a port, server_name and 1 location.
	for (int i = 0; i <= *servers; i++)
	{
		std::map<std::string, t_locations>::iterator it = serverConfig[i].locations.begin();
		if (it == serverConfig[i].locations.end() || !serverConfig[i].port || serverConfig[i].upload_path.empty() == 1 || serverConfig[i].server_name[0].empty() == 1)
		{
			std::cerr << "ERROR : configuration file lacks mandatory informations" << std::endl;
			freeConfig(serverConfig, *servers);
			return (NULL);
		}
		if (!serverConfig[i].max_body_size)
			serverConfig[i].max_body_size = 1024;
		while (it != serverConfig[i].locations.end())
		{
			if (it->second.root_path.empty() == 1)
				it->second.root_path = "./var/www/html/";
			if (it->second.allowed_methods.begin() == it->second.allowed_methods.end())
			{
				std::map<std::string, bool> res;
				res["GET"] = false;
				res["POST"] = false;
				res["DELETE"] = false;
				it->second.allowed_methods = res;
			}
			++it;
		}
	}
	return (serverConfig);
}

t_server_scope		*confParser(std::string buffer, int *servers)
{
	t_server_scope	*serverConfig = NULL;
	for (int i = 0; buffer[i] && buffer[i] != 0; i++)
	{
		while (buffer[i] && buffer[i] == '\n')
			i++;
		if (!(serverConfig = parseServer(&i, buffer, serverConfig, servers)))
			return (NULL);
	}
	//Minimal config checker
	if (!(serverConfig = checkConfig(serverConfig, servers)))
		return (NULL);
	return (serverConfig);
}
