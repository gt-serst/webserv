#include "request.hpp"

//on devra check si on recoit bien toute la requete en un seul bloc
//si pas un fera un precheck pour l'envoyer ici en un coup

//only POST should have a body 

//SET FUNCTIONS

Request::Request(std::string& buffer/*, Server& server*/)
{
    // _server = server;
    setRequest(buffer);
    std::cout << "request created" << std::endl;
}

Request::~Request()
{
    std::cout << "Request destroyed" << std::endl;
}

void Request::setRequest(std::string& buffer)
{
    _request = buffer;
    setRequestMethod();
}

void Request::setRequestMethod(void)
{
    std::stringstream ss(_request);
    std::string line;

    std::getline(ss, line);
    std::istringstream firstLine(line);
    firstLine >> _request_method >> _path_to_file >> _version;

    if (_request_method.compare("GET") 
        && _request_method.compare("POST") 
        && _request_method.compare("DELETE"))
    {
        exit(0); //handle error
    }
    if (_version.compare(0, 4, "HTTP"))
        exit(0); //handle error
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

//GET FUNCTIONS

std::string Request::getRequestMethod()
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