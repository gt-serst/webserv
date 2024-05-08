#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <exception>

enum state //state is current state so the name is what we previsly validated || when we encounter any (% hex hex) char we should translate hexa to ascii
{
    R_line,
    R_method,
    R_first_space,
    R_uri, // checking if the first char is not /
    R_uri_absolute, //checking for schema only accept http/s
    R_abs_http_s //checking for :
    R_abs_schema_end //checking for //
    R_abs_slashes //checking if first char after // is [ or not
    R_abs_literal_ip //parse literal ip and check : (port) stop when ]
    R_abs_host_start //check for characters validity and check : (port)
    R_abs_port //parse port check if /
    R_uri_after_slash, //after the / of the root
    R_uri_query //if ? in the path 
    R_second_space
    R_version //will only accept HTTP/1.1
    


}


class Request
{
    private:
        std::string _request;
        //Server _server;
        std::string _request_method;
        std::string _path_to_file;
        std::string _version;
        std::map<std::string, std::string> _headers;
        std::string _body;
        int _len;

        void parseRequestLine(void);
        void setRequestMethod(void);
        void setHeader(std::stringstream& ss, std::streampos startpos);
        void setBody(std::stringstream& ss, std::streampos startpos);
        // void setLen(std::string _request);

        std::string getRequestMethod();
        std::string getPathToFile();
        std::string getVersion();
        // std::string getHeader();
        // std::string getBody();
        // int getLen();

    public:
        Request(std::string& buffer/*, Server& server*/);
        ~Request();

        void setRequest(std::string& buffer);
        //std::string getRequest();
};

#endif
