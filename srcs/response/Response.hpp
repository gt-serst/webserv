/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 16:12:17 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/23 17:12:31 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "../parser/confParser.hpp"
# include "../request/Request.hpp"
# include <string>
# include <map>
# include <dirent.h>

class Response{

	public:
		void	handleDirective(std::string path, t_locations loc, std::map<std::string, t_locations> routes, Request *req);

	private:
		Response();
		~Response();
		bool	isLocationRooted(std::string& path, std::string root, t_locations loc);
		void	getFileType(std::string path, t_locations loc, std::map<std::string, t_locations> routes, Request *req);
		void	isDir(std::string path, t_locations loc, std::map<std::string, t_locations> routes, Request *req);
		void	isFile(std::string path, t_locations loc, Request *req);
		bool	isMethodAllowed(t_locations loc, Request *req);
		void	runDirMethod(std::string path, t_locations loc, Request *req);
		void	isAutoIndex(DIR *dr, std::string path, t_locations loc);
		void	deleteDir(DIR *dr, std::string path);
		void	isCGI(std::string path, t_locations loc, Request *req);
		void	runFileMethod(std::string path, t_locations loc, Request *req);
		void	openFile(std::string path);
		void	uploadFile(std::string path, Request *req);	
		void	deleteFile(std::string path);
		std::string	_response;
};

#endif
