#include "request.hpp"

//on devra check si on recoit bien toute la requete en un seul bloc
//si pas un fera un precheck pour l'envoyer ici en un coup

//only POST should have a body 

//SET FUNCTIONS

bool    allowedCharURI(char ch)
{
    if ((ch >= '#' && ch <= ';') || (ch >= '?' && ch <= '[') || (ch >= 'a' && ch <= 'z') ||
       ch == '!' || ch == '=' || ch == ']' || ch == '_' || ch == '~')
        return (true);
    return (false);
}

Request::Request(std::string& buffer/*, Server& server*/)
{
    // _server = server;
    setRequest(buffer);
    std::cout << "Parsing request" << std::endl;
}

Request::~Request()
{
    std::cout << "Request destroyed" << std::endl;
}

void Request::setRequest(std::string& buffer)
{
    _request = buffer;
	std::stringstream ss(_request);
	std::string extract_line;
	std::getline(ss, extract_line);
    parseRequestLine(extract_line.c_str());
    //setRequestMethod();
}

void Request::parseRequestLine(const char *line)
{
	int len = strlen(line);
	int state = R_line;
	int path_start = 0;

	for (int i = 0; i < len; i++) //for security I should be goint to the begin of loop / switch after uptating each state because of i
	{
		std::cout << "hello" << std::endl;
		// if (i > MAX_URI_SIZE)
		// 	return ; //error URI too long
		switch (state)
		{
			case R_line: //checking for the method not accepting leading WS
			{
				if (strncmp(line, "GET", 3) == 0)
				{
					_request_method = GET;
					i += 2;
					state = R_method;
				}
				else if (strncmp(line, "POST", 4) == 0)
				{
					_request_method = POST;
					i += 3;
					state = R_method;
				}
				else if (strncmp(line, "DELETE", 6) == 0)
				{
					_request_method = DELETE;
					i += 5;
					state = R_method;
				}
				else
					return ; //error 400 bad request : method if we want to be picky it could be a 405 : method not available if they enter another existing but unsported method
				break ;
			}
			case R_method: //checking for a single space after method
			{
				if (line[i] == ' ')
					state = R_first_space;
				else
					return ; //error 400 bad request : first space
				break ;
			}
			case R_first_space: //checking if we have absolute or relative path 
			{
				if (line[i] == '/')
				{
					path_start = i;
					state = R_uri_after_slash;
				}
				else if (strncmp(&line[i], "http://", 7)) //using strncmp by passing the adress of the first char &line[i]
				{
					i += 6;
					state = R_abs_slashes;
				}
				else if (strncmp(&line[i], "https://", 8))
				{
					i += 7;
					state = R_abs_slashes;
				}
				else
					return ; //error 400 bad request
				break ;
			}
			case R_abs_slashes:
			{
				if (line[i] == '[')
				{
					state = R_abs_literal_ip;
				}
				else
					state = R_abs_host_start;
				break ;
			}
			case R_abs_literal_ip: //devoir stocker 
			{
				if (!isdigit(line[i]) || line[i] != '.' || line[i] != ']')
					return ; //error 400 bad request : unsuported litteral ip 
				if (line[i] == ']')
					state = R_abs_host_end;
				break ;
			}
			case R_abs_host_start:
			{
				if (!allowedCharURI(line[i]))
					return ; //error bad request
				else if (line[i] == ':')
					state = R_abs_port;
				else if (line[i] )
				break ;
			}
			case R_abs_host_end:
			{
				if (line[i] == ':')
					state = R_abs_port;
				else if (line[i] == '/')
					state = R_abs_path;
				else
					return ; //error
				break ;
			}
			case R_abs_port:
			{
				if (line[i] == '/')
					state = R_abs_path;
				else if (line[i] == ' ')
					state = R_second_space;
				else if(!isdigit(line[i]))
					return ; //error port
				break ;
			}
			case R_abs_path:
			{
				if (line[i] == '?')
					state = R_uri_query;
				if (line[i] == '#')
					state = R_fragment ; //manage fragment
				else if (line[i] == ' ')
					state = R_second_space;
				break ;
			}
			case R_second_space:
			{
				if (strncmp(&line[i], "HTTP/1.1", 8) != 0)
					return ; //error server only supports http 1.1
				else
					state = R_version;
				break ;
			}
			case R_version:
			{
				if (line[i] == '\r')
					state = R_cr;
				else
					return ; //error;
				break ;
			}
			case R_cr:
			{
				if (line[i] == '\n' )
					state = R_crlf;
				else
					return ; //error;
				break ;
			}
			case R_crlf:
			{
				if (i == len)
					return ; //good
				else
					return ; //error
			}
		}
	}	
}

void Request::setRequestMethod(void)
{
    std::stringstream ss(_request);
    std::string line;

    std::getline(ss, line);
    std::istringstream firstLine(line);

    if (_version.compare(0, 8, "HTTP/1.1"))
        exit(0); //handle error (this server only support http 1.1 requests)
    if (_path_to_file.compare(0, 7, "http://") && _path_to_file.compare(0, 1, "/")
        && _path_to_file.compare(0, 8, "https://"))
        exit(0); // handle error
    std::cout << "Method: " << _request_method << std::endl;
    std::cout << "Path: " << _path_to_file << std::endl;
    std::cout << "Version: " << _version << std::endl;
    std::streampos currentpos = ss.tellg();
    setHeader(ss, currentpos);
}

void Request::setHeader(std::stringstream& ss, std::streampos startpos)
{
    std::cout << "headers" << std::endl;
    ss.seekg(startpos);
    std::string line;

    while (std::getline(ss, line) && !line.empty())
    {
        size_t pos = line.find(':');
        if (pos != std::string::npos) 
        {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            _headers[key] = value;
        }
    }

    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }
}

void Request::setBody(std::stringstream& ss, std::streampos startpos)
{
    std::cout << "Body" << std::endl;

    ss.seekg(startpos);
    std::string line;

    
}

//GET FUNCTIONS

t_method Request::getRequestMethod()
{
    return _request_method;
}

std::string Request::getPathToFile()
{
    return _path_to_file;
}

std::string Request::getVersion()
{
    return _version;
}