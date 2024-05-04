#include "request.hpp"

Request::Request(std::string& buffer)
{
    setRequest(buffer)
    std::cout << "request created" << std::endl;
}

Request::~Request()
{
    std::cout << "Request destroyed" << std::endl;
}

void Request::setRequest(std::string& buffer)
{
    _request = buffer;

    
}