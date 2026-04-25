/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: a.. <adahadda@student.1337.ma>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 12:00:00 by a                 #+#    #+#             */
/*   Updated: 2026/04/25 12:00:00 by a..              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include "codexion.h"

int	is_numeric_string(char *str)
{
	int	i;

	i = 0;
	if (!str || str[0] == '\0')
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

int	check_death(t_table *table)
{
	int	status;

	pthread_mutex_lock(&table->write_lock);
	status = table->simulation_dead;
	pthread_mutex_unlock(&table->write_lock);
	return (status);
}

long long	get_current_time(t_table *table)
{
	struct timeval	tv;

	(void)table;
	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

/*
** simulate_sleep: Sleeps for ms milliseconds in small increments,
** breaking early if the simulation has ended.
*/
void	simulate_sleep(long long ms, t_table *table)
{
	long long	start;

	(void)table;
	start = get_current_time(NULL);
	while ((get_current_time(NULL) - start) < ms)
	{
		if (check_death(table) == 1)
			break ;
		usleep(500);
	}
}

/*
** print_status: Prints a coder state message under write_lock.
** FIX: Only "burned out" sets simulation_dead. "Finished" was an
** invalid log message removed from the codebase entirely.
*/
void	print_status(t_coder *coder, char *status)
{
	long long	timestamp;

	pthread_mutex_lock(&coder->table->write_lock);
	if (coder->table->simulation_dead == 1)
	{
		pthread_mutex_unlock(&coder->table->write_lock);
		return ;
	}
	timestamp = get_current_time(coder->table) - coder->table->start_time;
	printf("%lld %lld %s\n", timestamp, coder->id, status);
	if (strcmp(status, "burned out") == 0)
		coder->table->simulation_dead = 1;
	pthread_mutex_unlock(&coder->table->write_lock);
}
