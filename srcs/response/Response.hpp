/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 16:12:17 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/27 17:20:09 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

//# include "../parser/confParser.hpp"
# include "../request/Request.hpp"
# include <string>
# include <map>
# include <sys/types.h>
# include <dirent.h>


class Response{

	typedef enum e_file
	{
		DIR,
		FILE,
		UNKNOWN
	}	t_file;

	typedef enum e_file_type
	{
		PNG,
		JPEG,
		SVG,
		GIF,
		PDF,
		ZIP,
		MP4,
		TXT
	}	t_file_type;

	public:
		Response();
		~Response();
		void		handleDirective(std::string path, t_locations loc, std::map<std::string, t_locations> routes, Request req, std::map<int, std::string> error_paths);
		void		errorResponse(int error_code, std::string message, std::string path);
		std::string	_response;

	private:
		bool		attachRootToPath(std::string& path, std::string root, t_locations loc);
		void		getFileType(std::string path, t_locations loc, std::map<std::string, t_locations> routes, Request req);
		void		isDir(std::string path, t_locations loc, std::map<std::string, t_locations> routes, Request req);
		void		isFile(std::string path, t_locations loc, Request req);
		bool		isMethodAllowed(t_locations loc, Request req);
		void		runDirMethod(std::string path, t_locations loc, Request req);
		void		isAutoIndex(::DIR *dr, std::string path, t_locations loc);
		void		deleteDir(::DIR *dr, std::string path);
		void		isCGI(std::string path, t_locations loc, Request req);
		void		runFileMethod(std::string path, t_locations loc, Request req);
		void		downloadFile(std::string path);
		void		uploadFile(std::string path, Request req);
		void		deleteFile(std::string path);
		void		downloadFileResponse(std::string stack);
		std::string	getContentType(std::string stack);
		t_file_type	stringToEnum(std::string const& str);
		void		uploadFileResponse(void);
		void		autoIndexResponse(std::string dir_list);
		void		deleteResponse(void);
		void		generateResponse(void);

		std::string	_http_version;
		std::string	_status_code;
		std::string	_status_message;
		std::string	_content_type;
		std::string	_content_len;
		std::string	_body;
};

#endif
