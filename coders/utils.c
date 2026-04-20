/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: a.. <adahadda@student.1337.ma>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 12:00:00 by a                 #+#    #+#             */
/*   Updated: 2026/04/20 13:42:16 by a..              ###   ########.fr       */
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

long long get_current_time(t_table *table)
{
	struct timeval	tv;

	(void)table; 
	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void simulate_sleep(long long ms, t_table *table)
{
	long long	start;

	(void)table;
	start = get_current_time(NULL);
	// We check the real clock in a loop, taking tiny micro-naps 
	// to let the OS schedule other Coders!
	while ((get_current_time(NULL) - start) < ms)
	{
		usleep(500); 
	}
}

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
	if (strcmp(status, "died") == 0 || strcmp(status, "Finished") == 0)
		coder->table->simulation_dead = 1;
	pthread_mutex_unlock(&coder->table->write_lock);
}
