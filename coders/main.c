/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: a.. <adahadda@student.1337.ma>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 12:00:00 by a                 #+#    #+#             */
/*   Updated: 2026/04/25 00:43:52 by a..              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include "codexion.h"

static void	safe_exit(t_table table, t_rules rules)
{
	int	i;

	i = 0;
	while (i < rules.number_of_coders)
	{
		pthread_mutex_destroy(&table.dongles[i]);
		pthread_mutex_destroy(&table.coders[i].meal_lock);
		i++;
	}
	pthread_mutex_destroy(&table.write_lock);
	pthread_mutex_destroy(&table.table_lock);
	pthread_mutex_destroy(&table.time_lock);
	pthread_cond_destroy(&table.intercom);
	free(table.dongles);
	free(table.coders);
	free(table.heap.data);
	free(table.dongle_ready_time);
	free(table.state);
}

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
	safe_exit(my_table, my_rules);
	return (0);
}
