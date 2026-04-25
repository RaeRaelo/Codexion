/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: a.. <adahadda@student.1337.ma>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/25 00:39:45 by a..               #+#    #+#             */
/*   Updated: 2026/04/25 12:00:00 by a..              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long int	ft_atoll(char *str)
{
	int			i;
	long long	nbr;

	i = 0;
	nbr = 0;
	while (str[i])
	{
		if (str[i] >= '0' && str[i] <= '9')
			nbr = nbr * 10 + (str[i] - '0');
		else
			return (2147483647LL + 1);
		i++;
	}
	return (nbr);
}

void	get_dongle_indices(t_coder *coder, int *f, int *s)
{
	*f = coder->id - 1;
	*s = coder->id % coder->rules->number_of_coders;
	if (coder->id % 2 != 0)
	{
		*f = coder->id % coder->rules->number_of_coders;
		*s = coder->id - 1;
	}
}

void	wait_cooldown(t_table *table, int d_idx)
{
	long long	ready;
	long long	now;

	pthread_mutex_lock(&table->time_lock);
	ready = table->dongle_ready_time[d_idx];
	pthread_mutex_unlock(&table->time_lock);
	now = get_current_time(table);
	if (now < ready)
		simulate_sleep(ready - now, table);
}

void	post_compile_cycle(t_coder *coder)
{
	print_status(coder, "is debugging");
	simulate_sleep(coder->rules->time_to_debug, coder->table);
	print_status(coder, "is refactoring");
	simulate_sleep(coder->rules->time_to_refactor, coder->table);
}

void	*handle_single_coder(t_coder *coder)
{
	pthread_mutex_lock(&coder->dongles[0]);
	print_status(coder, "has taken a dongle");
	simulate_sleep(coder->rules->time_to_burnout, coder->table);
	pthread_mutex_unlock(&coder->dongles[0]);
	return (NULL);
}
