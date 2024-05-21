#include "request.hpp"

//on devra check si on recoit bien toute la requete en un seul bloc
//si pas un fera un precheck pour l'envoyer ici en un coup

//only POST should have a body 

//SET FUNCTIONS

// int setPort_andCheck(const char *line, int start, int end)
// {
// 	return ;
// }

std::string convert_charptr_string(const char *line, int start, int end)
{
    char *extract = new char[end - start + 1];
    strncpy(extract, &line[start], end - start);
    extract[end - start] = '\0';
    std::string retval(extract);
    delete[] extract;
    return retval;
}

bool	unreserved_char(char ch)
{
	if (isalnum(ch) || ch == '-' || ch == '_' || ch == '.' || ch == '!'
		|| ch == '~' || ch == '*' || ch == '\'' || ch == '(' || ch == ')' || ch == '=')
		return true;
	else
		return false;
}

bool    allowedCharURI(char ch)
{
    if ((ch >= '#' && ch <= ';') || (ch >= '?' && ch <= '[') || (ch >= 'a' && ch <= 'z') ||
       ch == '!' || ch == '=' || ch == ']' || ch == '_' || ch == '~')
        return (true);
    return (false);
}

Request::Request(std::string& buffer/*, Server& server*/)
{
    std::cout << "Parsing request" << std::endl;
    // _server = server;
	_version = "HTTP/1.1";
	_path_to_file = "/";
	_hostname = "";
	_body = "";
	_query = "";
	_error_code = -1;
	state = R_line;
    setRequest(buffer);
}

Request::~Request()
{
	std::cout << "Printing request params" << std::endl;
	std::cout << "Method == " << _request_method << std::endl;
	std::cout << "Path == " << _path_to_file << std::endl;
	std::cout << "Query == " << _query << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    std::cout << "Request destroyed" << std::endl;
}

void Request::setRequest(std::string& buffer)
{
    _request = buffer;
	std::stringstream ss(_request);
	std::string extract_line;
	std::getline(ss, extract_line, '\n');
	extract_line += '\n';
    parseRequestLine(extract_line.c_str());
	std::streampos pos = ss.tellg();
    pos = setHeader(ss, pos);
	if (state == R_done)
		return ;
	setBody(ss, pos);
}

void Request::parseRequestLine(const char *line)
{
	int len = strlen(line);
	
	int start = 0;

	for (int i = 0; i <=len; i++) //for security I should be goint to the begin of loop / switch after uptating each state because of i
	{
		//std::cout << "hello : i == " << i << " state == " << state << " char == " << line[i] << " ascii value :" << (int)line[i] << std::endl;
		if (i > MAX_URI_SIZE)
			return ; //error URI too long
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
				{
					std::cout << "Error : method" << std::endl;
					return ; //error 400 bad request : method if we want to be picky it could be a 405 : method not available if they enter another existing but unsported method
				}
				break ;
			}
			case R_method: //checking for a single space after method
			{
				if (line[i] == ' ')
					state = R_first_space;
				else
				{
					std::cout << "Error : first space" << std::endl;
					return ; //error 400 bad request : first space
				}
				break ;
			}
			case R_first_space: //checking if we have absolute or relative path 
			{
				if (line[i] == '/')
				{
					start = i;
					state = R_abs_path;
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
				{
					std::cout << "Error : path" << std::endl;
					return ; //error 400 bad request
				}
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
				if (line[i] == ' ')
					state = R_second_space;
				else if (!allowedCharURI(line[i]))
				{
					std::cout << "Error : URI char" << std::endl;
					return ; //error bad request
				}
				else if (line[i] == ':')
				{
					// start = i;
					state = R_abs_port;
				}
				break ;
			}
			case R_abs_host_end:
			{
				if (line[i] == ':')
				{
					// start = i;
					state = R_abs_port;
				}
				else if (line[i] == '/')
				{
					start = i;
					state = R_abs_path;
				}
				else
				{
					std::cout << "Error : host end" << std::endl;
					return ; //error
				}
				break ;
			}
			case R_abs_port:
			{
				if (line[i] == '/')
				{
					// _port = setPort_andCheck(line, start, i);
					start = i;
					state = R_abs_path;
				}
				else if (line[i] == ' ')
				{
					// _port = setPort_andCheck(line, start, i);
					// start = 0;
					state = R_second_space;
				}
				else if(!isdigit(line[i]))
				{
					std::cout << "Error : port" << std::endl;
					return ; //error port
				}
				break ;
			}
			case R_abs_path:
			{
				if (line[i] == '?')
				{
					_path_to_file = convert_charptr_string(line, start, i);
					start = i + 1;
					state = R_uri_query;
				}
				if (line[i] == '#')
				{
					start = i + 1;
					state = R_fragment ; //manage fragment
				}
				else if (line[i] == ' ')
				{
					_path_to_file = convert_charptr_string(line, start, i);
					start = 0;
					state = R_second_space;
				}
				break ;
			}
			case R_second_space:
			{
				if (strncmp(&line[i], "HTTP/1.1", 8) != 0)
				{
					std::cout << "Error : version" << std::endl;
					return ; //error server only supports http 1.1
				}
				else
				{
					i += 7;
					state = R_version;
				}
				break ;
			}
			case R_version:
			{
				std::cout << i << std::endl;
				if (line[i] == '\r')
					state = R_cr;
				else
				{
					std::cout << "Error : CR" << std::endl;
					return ; //error;
				}
				break ;
			}
			case R_cr:
			{
				std::cout << "ender" << std::endl;
				if (line[i] == '\n' )
					state = R_crlf;
				else
				{
					std::cout << "Error : LF" << std::endl;
					return ; //error;
				}
				break ;
			}
			case R_crlf:
			{
				std::cout << " crlf" << std::endl;
				if (i == len)
				{
					std::cout << "First line parsing succesfull" << std::endl;
					return ; //good
				}
				else
				{
					std::cout << "Error : CRLF" << std::endl;
					return ; //error
				}
			}
			case R_uri_query:
			{
				if (line[i] == ' ')
				{
					_query = convert_charptr_string(line, start, i);
					start = 0;
					state = R_second_space;
				}
				else if (line[i] == '#')
				{
					_query = convert_charptr_string(line, start, i);
					start = i + 1;
					state = R_fragment;
				}
				// else if (line[i] == '%')
				// {
				// 	manage_hex_conv(&line, i);
				// }
				else if (unreserved_char(line[i]) || line[i] == '&')
				{
					break ;
				}
				else
					return ; //error
				break ;
			}
		}
	}
	std::cout << "Error : fatal state == " << state << std::endl;
	return ;
}

std::streampos Request::setHeader(std::stringstream& ss, std::streampos startpos)
{
    std::cout << "headers" << std::endl;
    ss.seekg(startpos);
    std::string line;

    while (std::getline(ss, line, '\n') && !line.empty())
    {
		if (line.compare("\r") == 0)
		{
			state = R_headers;
			return(ss.tellg());
		}
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
	state = R_done;
	return 0;
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
