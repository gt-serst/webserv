#include "confParser.hpp"

static void		freeConfig(t_server_scope *serverConfig, int servers)
{
	(void)servers;
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

static t_server_scope *isServerCGI(int *i, std::string buffer, t_server_scope *serverConfig, int *servers)
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
}

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

static t_server_scope	*isServerLocation(int *i, std::string buffer, t_server_scope *serverConfig, int *servers)
{
	(void)servers;
	std::cout << "I : " << *i << std::endl;
	std::cout << "buffer[*i] = " << buffer[*i] << std::endl;
	return (serverConfig);
}

static t_server_scope	*getServerConfig(int *i, std::string buffer, t_server_scope *serverConfig, int *servers)
{
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
		std::cout << buffer[*i] << std::endl;
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
				if (!(serverConfig = isServerLocation(i, buffer, serverConfig, servers)))
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
	*i += 2;
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
	t_server_scope *temp = new t_server_scope[*servers];
	if (*servers > 1) {
		for (int j = 0; j < *servers - 1; ++j) {
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

t_server_scope		*confParser(std::string buffer)
{
	t_server_scope	*serverConfig = NULL;
	int		servers = 0;
	for (int i = 0; buffer[i]; i++)
	{
		if (!(serverConfig = parseServer(&i, buffer, serverConfig, &servers)))
			return (NULL);
	}
	delete[] serverConfig;
	std::cout << buffer << std::endl;
	return (serverConfig);
}
