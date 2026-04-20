/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: a.. <adahadda@student.1337.ma>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 12:00:00 by a                 #+#    #+#             */
/*   Updated: 2026/04/20 13:44:07 by a..              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include "codexion.h"


int	main(int argc, char **argv)
{
	t_rules	my_rules;
	t_table	my_table;

	if (argc != 9)
	{
		printf("Error: Invalid number of arguments.\n");
		return (1);
	}
	if (init_rules(&my_rules, argv) == 1)
	{
		printf("Error: Invalid argument detected.\n");
		return (1);
	}
	if (init_table(&my_table, &my_rules) == 1)
	{
		printf("Error: Table initialization failed.\n");
		return (1);
	}
	start_simulation(&my_table);
	return (0);
}
