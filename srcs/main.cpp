/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 10:35:22 by gt-serst          #+#    #+#             */
/*   Updated: 2024/05/21 16:10:58 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec/ServerManager.hpp"

int	main(int argc, char **argv)
{
	ServerManager	sm;

	(void)argc, (void)argv;
	sm.launchServer();
}
