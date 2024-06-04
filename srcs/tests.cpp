#include "request.hpp"

int main()
{
    std::string getRequest = "GET https://lol:80/path/reso|rce?value=papa&hello=lol HTTP/1.1\r\n"
                             "Host: localhost:4242\r\n"
                             "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:121.0) Gecko/20100101 Firefox/121.0\r\n"
                             "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\r\n"
                             "Accept-Language: en-US,en;q=0.5\r\n"
                             "Accept-Encoding: gzip, deflate, br\r\n"
                             "Connection: keep-alive\r\n"
                             "Upgrade-Insecure-Requests: 1\r\n"
                             "Sec-Fetch-Dest: document\r\n"
                             "Sec-Fetch-Mode: navigate\r\n"
                             "Sec-Fetch-Site: none\r\n"
                             "Sec-Fetch-User: ?1\r\n";
    std::string postRequest = 
    "POST /api/login HTTP/1.1\r\n"
    "Host: example.com\r\n"
    "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:97.0) Gecko/20100101 Firefox/97.0\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: 43\r\n"
    "\r\n"
    "{\"username\": \"john_doe\", \"password\": \"password123\"}"
    "\r\n";
    Request test(getRequest);

    return 0;
}