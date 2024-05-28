/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 16:12:17 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/28 15:43:28 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "../parser/confParser.hpp"
# include "../request/Request.hpp"
# include <string>
# include <map>
# include <sys/types.h>
# include <dirent.h>

typedef enum e_file
{
	E_DIR,
	E_FILE,
	E_UNKNOWN
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

class Response{

	public:
		Response();
		~Response();
		void		handleDirective(std::string path, t_locations loc, std::map<std::string, t_locations> routes, Request req, std::map<int, std::string> error_paths);
		void		errorResponse(int error_code, std::string message, std::string path);
		std::string	getResponse() const;

	private:
		bool		attachRootToPath(std::string& path, std::string root);
		int			getFileType(std::string path);
		bool		findIndexFile(std::string& path, t_locations& loc, std::map<std::string, t_locations> routes);
		void		fileRoutine(std::string path, t_locations loc, Request req);
		bool		isMethodAllowed(t_locations loc, Request req);
		void		runDirMethod(std::string path, t_locations loc, Request req);
		void		isAutoIndex(std::string path, t_locations loc);
		void		uploadDir(std::string path);
		void		deleteDir(std::string path);
		bool		findCGI(std::string cgi_path);
		void		runFileMethod(std::string path, Request req);
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
		std::string	_response;
		std::string	_http_version;
		std::string	_status_code;
		std::string	_status_message;
		std::string	_content_type;
		std::string	_content_len;
		std::string	_body;
};

#endif
