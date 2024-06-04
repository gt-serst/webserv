#ifndef REQUEST_H
#define REQUEST_H

#include "../exec/Server.hpp"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <exception>
#include <string.h>
#include <limits.h>
#define MAX_URI_SIZE 200
#define MAX_REQUEST_SIZE

enum state //state is current state so the name is what we previsly validated || when we encounter any (% hex hex) char we should translate hexa to ascii
{
	R_line,
	R_method,
	R_first_space,
	R_uri, // checking if the first char is not /
	R_uri_absolute, //checking for schema only accept http/s
	R_abs_http_s, //checking for :
	R_abs_schema_end, //checking for //
	R_abs_slashes, //checking if first char after // is [ or not
	R_abs_literal_ip, //parse literal ip and check : (port) stop when ]
	R_abs_host_end,
	R_abs_host_start, //check for characters validity and check : (port)
	R_abs_port, //parse port check if /
	R_abs_path,
	R_uri_after_slash, //after the / of the root
	R_uri_query, //if ? in the path
	R_second_space,
	R_version,
	R_version_major,
	R_version_dot,
	R_version_minor,
	R_version_done, //will only accept HTTP/1.1
	R_cr,
	R_crlf,
	R_fragment,
	R_headers,
	R_chunked_start,
	R_chunk_size,
	R_chunk_cr,
	R_chunk_lf,
	R_chunk_content,
	R_chunk_content_cr,
	R_chunk_content_lf,
	R_chunk_done,
	R_body,
	R_error,
	R_done
};


class Request
{
	private:
		std::string _request;
		std::string _request_method;
		std::string _hostname;//std
		std::string _path_to_file;//std
		std::string _version;
		std::map<std::string, std::string> _headers;
		std::map<std::string, std::string> _query_args;
		std::string _body;
		std::string _query_str;
		std::string _error_msg;
		std::string _litteral_ip;
		int _error_code;
		int	_port;
		int _body_len;
		int state;
		int chunk_size;
		bool chunked;

		void parseRequestLine(char *line);
		std::streampos setHeader(std::stringstream& ss, std::streampos startpos);
		void setBody(std::stringstream& ss, std::streampos startpos);
		std::string	standardise(std::string str);
		void validity_checks();
		void manage_chunks(char *chunk);
		bool	handle_query();

	public:
		Request();
		Request(std::string& buffer, Server& server);
		~Request();

		void	setPathToFile(const std::string& path_to_file);
		void	setRequest(std::string& buffer);
		std::string	getRequest() const;
		std::string	getRequestMethod() const;
		std::string	getPathToFile() const;
		std::string	getVersion() const;
		std::string	getHeader(const std::string& key) const;
		std::string	getBody() const;
		std::string	getErrorMsg() const;
		std::string getHost() const;
		int			getErrorCode() const;
		int			getPort() const;
		std::string getQuerystr() const;
		Server _server;
		// int getLen();
};

#endif
