/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 16:12:17 by gt-serst          #+#    #+#             */
/*   Updated: 2024/06/14 12:21:52 by gt-serst         ###   ########.fr       */
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
		void		generateResponse(void);
		void		errorResponse(int error_code, std::string message, std::map<int, std::string> error_paths);
		void		setVersion(std::string version);
		void		setLocation(std::string location);
		void		setRedir(bool redir);
		std::string	getResponse(void) const;
		int			getStatusCode(void) const;
		std::string	getLocation(void) const;
		bool		getRedir(void) const;

	private:
		bool		rootPaths(t_locations loc, std::string& path, std::string upload_path, std::map<int, std::string> error_paths, Request& req);
		bool		attachRootToPath(std::string& path, std::string root);
		int			getFileType(struct stat buf);
		bool		findDefaultFile(std::string& path, t_locations& loc, std::map<std::string, t_locations> routes, Request& req, std::map<int, std::string> error_paths);
		void		fileRoutine(std::string path, std::map<int, std::string> error_paths, t_locations loc, Request& req);
		bool		isMethodAllowed(t_locations loc, Request& req);
		void		runDirMethod(std::string path, std::map<int, std::string> error_paths, t_locations loc, Request& req);
		void		isAutoIndex(std::string path, std::map<int, std::string> error_paths, t_locations loc, Request& req);
		bool		findCGI(std::map<std::string, std::string>	cgi_path, std::string path_to_file);
		void		handleCGI(std::string rootedpath, std::string path, Request& req, std::string exec_path);
		void		runFileMethod(std::string path, std::map<int, std::string> error_paths, Request& req);
		void		downloadFile(std::string path, std::map<int, std::string> error_paths);
		void		uploadQueryFile(std::string upload_path, std::map<int, std::string> error_paths, std::map<std::string, std::string> query, std::string body);
		void		uploadMultiformFile(std::string upload_path, std::map<int, std::string> error_paths, std::map<int, t_multi> multiform);
		void		deleteFile(std::string path, std::map<int, std::string> error_paths);
		void		autoIndexResponse(std::string path, std::string dir_list, Request& req);
		std::string	getCharCount(struct stat file_info);
		void		insertHtmlIndexLine(std::string redirect_url, std::string txt_button, std::string creation_date, std::string char_count);
		void		deleteResponse(void);
		void		downloadFileResponse(std::string stack);
		bool		checkContentType(std::string path);
		std::string	getContentType(std::string stack);
		t_file_type	stringToEnum(std::string const& str);
		void		uploadFileResponse(void);
		std::string	matchErrorCodeWithPage(int error_code, std::map<int, std::string> error_paths);
		void		createHtmlErrorPage(int error_code, std::string message);
		void		fileNotFound(void);
		bool		checkFileAccess(std::string path, std::map<int, std::string> error_paths, std::string perm);
		bool		checkRootAccess(std::string path);
		bool		checkErrorFileAccess(int error_code, std::string message, std::string error_path);
		std::string	_response;
		std::string	_http_version;
		int			_status_code;
		std::string	_status_message;
		std::string	_location;
		std::string	_content_type;
		int			_content_len;
		std::string	_body;
		bool		_redir;
};

#endif
