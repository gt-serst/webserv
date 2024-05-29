#include "Request.hpp"
#include "../exec/Server.hpp"

//on devra check si on recoit bien toute la requete en un seul bloc
//si pas un fera un precheck pour l'envoyer ici en un coup

//only POST should have a body

//SET FUNCTIONS

// void	Request::standardise(void)
// {

// }

int ip_checker(std::string& ip)
{
	int valid = 0;
	int test = 0;
	size_t len = ip.length();
	size_t pos;
	size_t tmp = 0;
	while ((pos = ip.find(".", tmp)) != std::string::npos && pos + 1 <= len)
	{
		test = std::stoi(ip.substr(tmp, len - pos));
		if (test > 255 || test < 0)
			break ;
		tmp = pos + 1;
		valid++;
	}
	if (valid == 3)
	{
		test = std::stoi(ip.substr(pos, len - 1));
		if (test <= 255 && test >= 0)
			valid++;
	}
	if (valid == 4)
		return 0;
	return 1;
}

int hexDigitToInt(char ch) 
{
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    return -1;
}

int	convert_hexa(char **line, int *i, int *len)
{
	if ((*i + 2) >= *len) return 1;

	if (isalnum(*line[*i + 1]) && isalnum(*line[*i + 2]))
	{
		int high = hexDigitToInt((*line)[*i + 1]);
        int low = hexDigitToInt((*line)[*i + 2]);
		if (high == -1 || low == -1) return 1;
		char convertedChar = (high << 4) | low;
		memmove(*line + *i + 1, *line + *i + 3, *len - *i - 2);
		(*line)[*i] = convertedChar;
		*len -= 2;
	}
	return 0;
}

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

bool	allowedCharURI(char ch)
{
	if ((ch >= '#' && ch <= ';') || (ch >= '?' && ch <= '[') || (ch >= 'a' && ch <= 'z') ||
		ch == '!' || ch == '=' || ch == ']' || ch == '_' || ch == '~')
		return (true);
	return (false);
}

Request::Request(){}

Request::Request(std::string& buffer, Server& server)
{
	std::cout << "Parsing request" << std::endl << std::endl;
	_server = server;
	_version = "";
	_path_to_file = "/";
	_hostname = "";
	_litteral_ip = "";
	_body = "";
	_query = "";
	_error_code = -1;
	_error_msg = "";
	state = R_line;
	setRequest(buffer);
	//standardise();
}

Request::~Request()
{
	if (state == R_error)
	{
		std::cout << "Error " << _error_code << " " << _error_msg << std::endl;
		return ;
	}
	/*std::cout << "Printing request params" << std::endl;
	std::cout << "Method == " << _request_method << std::endl;
	std::cout << "Path == " << _path_to_file << std::endl;
	std::cout << "Query == " << _query << std::endl;
	std::cout << "Version == " << _version << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}
	std::cout << "Request destroyed" << std::endl;*/
}

void Request::setRequest(std::string& buffer)
{
	_request = buffer;
	std::stringstream ss(_request);
	std::string extract_line;
	std::getline(ss, extract_line, '\n');
	extract_line += '\n';
	char *toparse = new char[extract_line.size() + 1];
	std::strcpy(toparse, extract_line.c_str());
	parseRequestLine(toparse);
	delete[] toparse;
	if (state == R_error)
		return ;
	std::streampos pos = ss.tellg();
	pos = setHeader(ss, pos);
	if (state == R_done || state == R_error)
		return ;
	setBody(ss, pos);
}

void Request::parseRequestLine(char *line)
{
	int len = strlen(line);
	int start = 0;
	if (len > MAX_URI_SIZE)
	{
		_error_msg = "Request-URI Too Large";
		_error_code = 414;
		state = R_error;
		return ;
	}

	for (int i = 0; i <=len; i++)
	{
		// if (line[i] == '%') //this should be elsewhere
		// {
		// 	if (convert_hexa(&line, &i, &len))
		// 	{
		// 		state = R_error;
		// 		return ;
		// 	}
		// }
		line[i] = line[i] | 0x20;
		switch (state)
		{
			case R_line: //checking for the method not accepting leading WS
			{
				if (strncmp(line, "GET", 3) == 0)
				{
					_request_method = "GET";
					i += 2;
					state = R_method;
				}
				else if (strncmp(line, "POST", 4) == 0)
				{
					_request_method = "POST";
					i += 3;
					state = R_method;
				}
				else if (strncmp(line, "DELETE", 6) == 0)
				{
					_request_method = "DELETE";
					i += 5;
					state = R_method;
				}
				else
				{
					_error_msg = "bad request : unsuported method";
					_error_code = 400;
					state = R_error;
					return ;
				}
				break ;
			}
			case R_method: //checking for a single space after method
			{
				if (line[i] == ' ')
					state = R_first_space;
				else
				{
					_error_msg = "Bad request : first space";
					_error_code = 400;
					state = R_error;
					return ; //error 400 bad request : first space
				}
				break ;
			}
			case R_first_space:
			{
				if (line[i] == '/')
				{
					start = i;
					state = R_abs_path;
				}
				else if (strncmp(&line[i], "http://", 7))
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
					_error_msg = "Bad request";
					_error_code = 400;
					state = R_error;
					return ;
				}
				break ;
			}
			case R_abs_slashes:
			{
				if (isdigit(line[i]))
				{
					start = i;
					state = R_abs_literal_ip;
				}
				else if (isalpha(line[i]) || line[i] == '_' || line[i] == '-')
				{
					state = R_abs_host_start;
					start = i;
				}
				else
				{
					state = R_error;
					_error_code = 400;
					_error_msg = "Bad request : hostname not valid";
					return ;
				}
				break ;
			}
			case R_abs_literal_ip: //devoir stocker
			{
				if (line[i] == ':')
				{
					_litteral_ip = convert_charptr_string(line, start, i);
					if (ip_checker(_litteral_ip))
					{
						state = R_error;
						_error_code = 400;
						_error_msg = "Bad IP adress, IPv6 is not supported";
						return ;
					}
					state = R_abs_port;
					start = i;
				}
				else if (line[i] == '/')
				{
					_litteral_ip = convert_charptr_string(line, start, i);
					if (ip_checker(_litteral_ip))
					{
						state = R_error;
						_error_code = 400;
						_error_msg = "Bad IP adress, IPv6 is not supported";
						return ;
					}
					start = i;
					state = R_abs_path;
				}
				else if (!isdigit(line[i]) || line[i] != '.')
				{
					_error_msg = "Bad request";
					_error_code = 400;
					state = R_error;
					return ; //error 400 bad request : unsuported litteral ip
				}
				break ;
			}
			case R_abs_host_start:
			{
				if (line[i] == ' ')
				{
					_hostname = convert_charptr_string(line, start, i);
					state = R_second_space;
				}
				else if (!allowedCharURI(line[i]))
				{
					_error_msg = "Bad request";
					_error_code = 400;
					state = R_error;
					return ; //error bad request
				}
				else if (line[i] == ':')
				{
					_hostname = convert_charptr_string(line, start, i);
					start = i + 1;
					state = R_abs_port;
				}
				break ;
			}
			case R_abs_host_end:
			{
				if (line[i] == ':')
				{
					start = i + 1;
					state = R_abs_port;
				}
				else if (line[i] == '/')
				{
					start = i;
					state = R_abs_path;
				}
				else
				{
					_error_msg = "Bad request";
					_error_code = 400;
					state = R_error;
					return ; //error
				}
				break ;
			}
			case R_abs_port:
			{
				if (line[i] == '/' && line[i - 1] != ':')
				{
					_port = std::stoi(convert_charptr_string(line, start, i));
					start = i;
					state = R_abs_path;
				}
				else if (line[i] == ' ')
				{
					_port = std::stoi(convert_charptr_string(line, start, i));
					state = R_second_space;
				}
				else if(!isdigit(line[i]))
				{
					_error_msg = "Bad request";
					state = R_error;
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
				else if (line[i] == '#')// on verra si c'est utile
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
				else if (!allowedCharURI(line[i]))
				{
					_error_msg = std::string("Bad character : '") + line[i] + '\'';
					_error_code = 400;
					state = R_error;
					return ;
				}
				break ;
			}
			case R_second_space:
			{
				if (strncmp(&line[i], "HTTP/", 5) != 0)
				{
					_error_msg = "Bad request : this server only handles HTTP requests";
					_error_code = 501;
					return ; 
				}
				else
				{
					i += 4;
					start = i;
					state = R_version_major;
				}
				break ;
			}
			case R_version_major:
			{
				if (line[i] != '1' &&
					line[i] != '2' &&
					line[i] != '3')
				{
					_error_code = 505;
					_error_msg = "HTTP Version not supported";
					return ;
				}
				start = i;
				state = R_version_dot;
				break ;
			}
			case R_version_minor:
			{
				if (!isdigit(line[i]))
				{
					_error_code = 400;
					_error_msg = "Bad request";
					state = R_error;
					return ;
				}
				state = R_version_done;
				break ;
			}
			case R_version_dot:
			{
				if (line[i] == '.')
				{
					state = R_version_minor;
					break ;
				}
				else
					state = R_version_done;
			}
			case R_version_done:
			{
				if (line[i] == '\r')
				{
					_version = convert_charptr_string(line, start, i);
					state = R_cr;
				}
				else
				{
					_error_msg = "Bad request";
					_error_code = 400;
					state = R_error;
					return ;
				}
				break ;
			}
			case R_cr:
			{
				if (line[i] == '\n' )
					state = R_crlf;
				else
				{
					_error_msg = "Bad request";
					_error_code = 400;
					state = R_error;
					return ;
				}
				break ;
			}
			case R_crlf:
			{
				if (i == len)
				{
					std::cout << "First line parsing succesfull" << std::endl;
					return ; //good
				}
				else
				{
					_error_msg = "Bad request";
					_error_code = 400;
					state = R_error;
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
				else if (unreserved_char(line[i]) || line[i] == '&')
				{
					break ;
				}
				else
				{
					_error_msg = "Bad request";
					_error_code = 400;
					state = R_error;
					return ; //error
				}
				break ;
			}
		}
	}
	return ;
}

void	Request::setPathToFile(const std::string& path_to_file)
{
	_path_to_file = path_to_file;
}

std::streampos Request::setHeader(std::stringstream& ss, std::streampos startpos)
{
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
		else
		{
			state = R_error;
			_error_msg = "Bad request : wrong header format";
			_error_code = 400;
			return 0;
		}
	}
	state = R_done;
	return 0;
}

void Request::setBody(std::stringstream& ss, std::streampos startpos)
{
	ss.seekg(startpos);
	std::string bodyContent((std::istreambuf_iterator<char>(ss)), std::istreambuf_iterator<char>());

	// Check if body ends with CRLF
	if (bodyContent.size() >= 2 && bodyContent.compare(bodyContent.size() - 2, 2, "\r\n") == 0)
	{
		_body = bodyContent;
		state = R_done; //
	}
	else
	{
		std::cerr << "Body does not end with CRLF" << std::endl;
		//set error and go next
	}
	return ;
}

//GET FUNCTIONS

std::string	Request::getRequest() const
{
	return _request;
}

std::string Request::getRequestMethod() const
{
	return _request_method;
}

std::string Request::getPathToFile() const
{
	return _path_to_file;
}

std::string Request::getVersion() const
{
	return _version;
}

std::string Request::getHeader(const std::string& key) const
{
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
	if (it != _headers.end())
	{
		return it->second;
	}
	else
	{
		return "";
	}
}

std::string Request::getBody() const
{
	return _body;
}

std::string	Request::getErrorMsg() const
{
	return _error_msg;
}

int	Request::getErrorCode() const
{
	return _error_code;
}

int Request::getPort() const
{
	return _port;
}

std::string Request::getHost() const
{
	return _hostname;
}
