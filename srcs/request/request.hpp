#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <iostream>

class Request
{
    private:
        std::string _request;
        Server _server;
        std::string _request_method;
        std::string _path_to_file;
        std::string _version;
        std::string _header;
        std::string _body;
        int _len;

        void setRequestMethod(std::string _request);
        void setPathToFile(std::string _request);
        void setVersion(std::string _request);
        void setHeader(std::string _request);
        void setBody(std::string _request);
        void setLen(std::string _request);

        std::string getRequestMethod();
        std::string getPathToFile();
        std::string getVersion();
        std::string getHeader();
        std::string getBody();
        int getLen();

    public:
        Request(std::string& buffer);
        ~Request();

        void setRequest(std::string& buffer);
        std::string getRequest();
};

#endif
