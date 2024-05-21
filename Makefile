# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/21 15:52:59 by gt-serst          #+#    #+#              #
#    Updated: 2024/05/21 15:56:59 by gt-serst         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

EXECUTION	=	srcs/exec/ServerManager.cpp\
				srcs/exec/Server.cpp\
				srcs/exec/Client.cpp

REQUEST		=	srcs/request/Request.cpp

SRCS		=	$(EXECUTION)\
				$(REQUEST)\
				main.cpp\

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
