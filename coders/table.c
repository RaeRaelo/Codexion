/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   table.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: a.. <adahadda@student.1337.ma>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 12:00:00 by a                 #+#    #+#             */
/*   Updated: 2026/04/25 12:00:00 by a..              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <pthread.h>
#include "codexion.h"

static int	init_coders(t_table *table, t_rules *rules)
{
	long long	i;

	i = 0;
	while (i < rules->number_of_coders)
	{
		table->coders[i].id = i + 1;
		table->coders[i].times_compiled = 0;
		table->coders[i].rules = rules;
		table->coders[i].dongles = table->dongles;
		table->coders[i].table = table;
		pthread_mutex_init(&table->dongles[i], NULL);
		pthread_mutex_init(&table->coders[i].meal_lock, NULL);
		i++;
	}
	return (0);
}

static int	alloc_table(t_table *table, t_rules *rules)
{
	table->coders = malloc(rules->number_of_coders * sizeof(t_coder));
	table->dongles = malloc(rules->number_of_coders * sizeof(pthread_mutex_t));
	table->heap.data = malloc(rules->number_of_coders * sizeof(t_request));
	table->dongle_ready_time = malloc(rules->number_of_coders
			* sizeof(long long));
	table->state = malloc(rules->number_of_coders * sizeof(int));
	if (!table->coders || !table->dongles || !table->heap.data
		|| !table->dongle_ready_time || !table->state)
	{
		free(table->coders);
		free(table->dongles);
		free(table->heap.data);
		free(table->dongle_ready_time);
		free(table->state);
		return (1);
	}
	return (0);
}

static void	setup_table(t_table *table, t_rules *rules)
{
	int	i;

	i = -1;
	while (++i < rules->number_of_coders)
	{
		table->dongle_ready_time[i] = 0;
		table->state[i] = 0;
	}
	pthread_mutex_init(&table->write_lock, NULL);
	pthread_cond_init(&table->intercom, NULL);
	pthread_mutex_init(&table->table_lock, NULL);
	pthread_mutex_init(&table->time_lock, NULL);
	table->ticket_dispenser = 0;
	table->heap.size = 0;
	table->simulation_dead = 0;
	table->rules = rules;
}

int	init_table(t_table *table, t_rules *rules)
{
	if (alloc_table(table, rules) == 1)
		return (1);
	setup_table(table, rules);
	init_coders(table, rules);
	return (0);
}

int	start_simulation(t_table *table)
{
	long long	i;
	pthread_t	monitor_thread;

	i = 0;
	table->start_time = get_current_time(NULL);
	while (i < table->rules->number_of_coders)
	{
		table->coders[i].last_compile_start = table->start_time;
		pthread_create(&table->coders[i].thread_id, NULL,
			coder_routine, &table->coders[i]);
		i++;
	}
	pthread_create(&monitor_thread, NULL, monitor_routine, table);
	i = 0;
	while (i < table->rules->number_of_coders)
	{
		pthread_join(table->coders[i].thread_id, NULL);
		i++;
	}
	pthread_join(monitor_thread, NULL);
	return (0);
}
