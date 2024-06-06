/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 16:12:17 by gt-serst          #+#    #+#             */
/*   Updated: 2024/06/06 11:47:12 by gt-serst         ###   ########.fr       */
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
	E_UNKN
}	t_file;

typedef enum e_file_type
{
	E_PNG,
	E_JPEG,
	E_SVG,
	E_GIF,
	E_PDF,
	E_ZIP,
	E_MP4,
	E_UNSUP
}	t_file_type;

class Response{

	public:
		Response(void);
		~Response(void);
		void		handleDirective(std::string path, t_locations loc, Request& req, Server& serv);
		bool		checkPortAndServerName(t_server_scope config);
		void		errorResponse(int error_code, std::string message, std::map<int, std::string> error_paths);
		std::string	getResponse() const;

	private:
		bool		attachRootToPath(std::string& path, std::string root, std::map<int, std::string> error_paths);
		int			getFileType(std::string path);
		bool		findIndexFile(std::string& path, t_locations& loc, std::map<std::string, t_locations> routes, Request& req);
		void		fileRoutine(std::string path, t_locations loc, Request& req, Server& serv);
		bool		isMethodAllowed(t_locations loc, Request& req);
		void		runDirMethod(std::string path, t_locations loc, Request& req, Server& serv);
		void		isAutoIndex(std::string path, t_locations loc, Request& req, std::map<int, std::string> error_paths);
		bool		findCGI(std::map<std::string, std::string>	cgi_path);
		void		runFileMethod(std::string path, Request& req, Server& serv);
		void		downloadFile(std::string path, std::map<int, std::string> error_paths);
		void		uploadFile(std::string path, std::string root, std::string upload_path, Request& req, Server& serv);
		void		deleteFile(std::string path, std::map<int, std::string> error_paths);
		void		autoIndexResponse(std::string path, std::string dir_list, Request& req);
		std::string	getCharCount(struct stat file_info);
		void		insertHtmlIndexLine(std::string redirect_url, std::string txt_button, std::string creation_date, std::string char_count);
		void		deleteResponse(void);
		void		downloadFileResponse(std::string stack, std::map<int, std::string> error_paths);
		std::string	getContentType(std::string stack);
		t_file_type	stringToEnum(std::string const& str);
		void		uploadFileResponse(void);
		void		generateResponse(void);
		std::string	matchErrorCodeWithPage(int error_code, std::map<int, std::string> error_paths);
		void		fileNotFound(void);
		bool		checkFileAccess(std::string path, std::map<int, std::string> error_paths);
		bool		checkRootAccess(std::string path);
		std::string	_response;
		std::string	_http_version;
		int			_status_code;
		std::string	_status_message;
		std::string	_content_type;
		int			_content_len;
		std::string	_body;
};

#endif
