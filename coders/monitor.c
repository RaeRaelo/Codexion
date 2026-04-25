/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: a.. <adahadda@student.1337.ma>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 12:00:00 by a                 #+#    #+#             */
/*   Updated: 2026/04/26 00:05:10 by a..              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <pthread.h>
#include "codexion.h"

/* check_coder_burnout: Checks if a coder has burned out */
void	check_coder_burnout(t_table *table, long long i)
{
	pthread_mutex_lock(&table->coders[i].meal_lock);
	if (table->rules->number_of_compiles_required > 0
		&& table->coders[i].times_compiled
		>= table->rules->number_of_compiles_required)
	{
		pthread_mutex_unlock(&table->coders[i].meal_lock);
		return ;
	}
	if ((get_current_time(table) - table->coders[i].last_compile_start)
		>= table->rules->time_to_burnout)
	{
		print_status(&table->coders[i], "burned out");
		pthread_mutex_lock(&table->table_lock);
		pthread_cond_broadcast(&table->intercom);
		pthread_mutex_unlock(&table->table_lock);
		pthread_mutex_unlock(&table->coders[i].meal_lock);
		return ;
	}
	pthread_mutex_unlock(&table->coders[i].meal_lock);
}

void	check_completion(t_table *table, long long i,
			long long *finished_coders)
{
	pthread_mutex_lock(&table->coders[i].meal_lock);
	if (table->rules->number_of_compiles_required > 0
		&& table->coders[i].times_compiled
		>= table->rules->number_of_compiles_required)
		(*finished_coders)++;
	if (*finished_coders == table->rules->number_of_coders)
	{
		pthread_mutex_unlock(&table->coders[i].meal_lock);
		pthread_mutex_lock(&table->write_lock);
		table->simulation_dead = 1;
		pthread_mutex_unlock(&table->write_lock);
		pthread_mutex_lock(&table->table_lock);
		pthread_cond_broadcast(&table->intercom);
		pthread_mutex_unlock(&table->table_lock);
		return ;
	}
	pthread_mutex_unlock(&table->coders[i].meal_lock);
}

/* monitor_loop: Main monitoring loop */
void	monitor_loop(t_table *table)
{
	long long	finished_coders;
	long long	i;

	while (1)
	{
		finished_coders = 0;
		i = 0;
		while (i < table->rules->number_of_coders)
		{
			check_coder_burnout(table, i);
			if (check_death(table))
				return ;
			check_completion(table, i, &finished_coders);
			if (check_death(table))
				return ;
			i++;
		}
		simulate_sleep(1, table);
	}
}

/* monitor_routine: Monitor thread entry point */
void	*monitor_routine(void *arg)
{
	t_table	*table;

	table = (t_table *)arg;
	monitor_loop(table);
	return (NULL);
}
