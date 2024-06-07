/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 15:19:55 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/24 17:56:43 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

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

t_file_type	stringToEnum(std::string const& str)
{
	if (str.compare(0, 20, "89 50 4e 47 d a 1a a") == 0) return (PNG);
	if (str.compare(0, 5, "ff d8") == 0) return (JPEG);
	if (str.compare(0, 23, "3c 3f 78 6d 6c 20 76 65") == 0) return (SVG);
	if (str.compare(0, 17, "47 49 46 38 39 61") == 0) return (GIF);
	if (str.compare(0, 11, "25 50 44 46") == 0) return (PDF);
	if (str.compare(0, 18, "50 4b 3 4 14 0 8 0") == 0) return (ZIP);
	if (str.compare(0, 20, "0 0 0 20 66 74 79 70") == 0) return (MP4);
	else
		return (TXT);
}

int	main(int argc, char **argv)
{
	if (argc > 1)
	{
		std::ifstream input(argv[1], std::ios::binary);
		if (input.is_open())
		{
			std::string	buffer;
			std::string	stack;

			while (std::getline(input, buffer))
			{
				stack += buffer;
				stack += '\n';
			}
			input.close();

			std::stringstream	ss;
			std::stringstream	ss_hex;
			std::vector<char>	bytes(8);
			std::string			octets;
			
			ss << stack;
			ss.read(&bytes[0], bytes.size());

			for (int i = 0; i < bytes.size(); i++)
				ss_hex << std::hex << (static_cast<int>(bytes[i]) & 0xFF) << " ";
			octets = ss_hex.str();
			std::cout << octets << std::endl;
			switch (stringToEnum(octets))
			{
				case PNG:
					std::cout << "This file is a png" << std::endl;
					break;
				case JPEG:
					std::cout << "This file is a jpeg" << std::endl;
					break;
				case SVG:
					std::cout << "This file is a svg" << std::endl;
					break;
				case GIF:
					std::cout << "This file is a gif" << std::endl;
					break;
				case PDF:
					std::cout << "This file is a pdf" << std::endl;
					break;
				case ZIP:
					std::cout << "This file is a zip" << std::endl;
					break;
				case MP4:
					std::cout << "This file is a mp4" << std::endl;
					break;
				case TXT:
					std::cout << "This file is a txt" << std::endl;
					break;
				default:
					perror("No file recognised");

			}
		}
		else
			std::cout << "Error: cannot open the file" << std::endl;
	}

	return (0);
}
