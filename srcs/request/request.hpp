#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <exception>

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

        // class RequestParseError : public std::exception {
        //     public : 
        //         const char* what() const noexcept override { return "Error request parsing"}
        // }
        void setRequestMethod(void);
        void setHeader(std::stringstream& ss, std::streampos startpos);
        // void setBody(std::string _request);
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
