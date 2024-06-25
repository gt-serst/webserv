# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gt-serst <gt-serst@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/21 15:52:59 by gt-serst          #+#    #+#              #
#    Updated: 2024/06/25 15:49:01 by gt-serst         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

UTILS		= srcs/utils/ft_atoi.cpp

PARSER		=	srcs/parser/confChecker.cpp\
				srcs/parser/confParser.cpp\
				srcs/webserv.cpp

EXECUTION	=	srcs/exec/ServerManager.cpp\
				srcs/exec/Server.cpp\

REQUEST		=	srcs/request/Request.cpp

RESPONSE	=	srcs/response/Response.cpp\
				srcs/response/Router.cpp\
				srcs/response/Cgi.cpp

SRCS		=	$(UTILS)\
				$(PARSER)\
				$(EXECUTION)\
				$(REQUEST)\
				$(RESPONSE)\
				srcs/main.cpp\

NAME		= webserv

FLAGS		= -Wall -Wextra -Werror -std=c++98

OBJS		= ${SRCS:.cpp=.o}

${NAME} : ${OBJS}
					c++ ${FLAGS} $(OBJS) -o $(NAME)

all		: ${NAME}

%.o:%.cpp
					c++ ${FLAGS} -c $< -o $@

clean	:
					rm -rf ${OBJS}

fclean	: clean
					rm -rf ${NAME}

re		: fclean all

.PHONY	: all clean fclean re NAME
