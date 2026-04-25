/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: a.. <adahadda@student.1337.ma>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/25 00:39:42 by a..               #+#    #+#             */
/*   Updated: 2026/04/25 12:00:00 by a..              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	can_eat(t_coder *c, long long my_p)
{
	int	l;
	int	r;
	int	i;

	l = c->id - 2;
	if (c->id == 1)
		l = c->rules->number_of_coders - 1;
	r = c->id % c->rules->number_of_coders;
	if (c->table->state[l] == 1 || c->table->state[r] == 1)
		return (0);
	i = -1;
	while (++i < c->table->heap.size)
	{
		if (c->table->heap.data[i].id - 1 != l
			&& c->table->heap.data[i].id - 1 != r)
			continue ;
		if (c->table->heap.data[i].p < my_p)
			return (0);
		if (c->table->heap.data[i].p == my_p
			&& c->table->heap.data[i].id > c->id)
			return (0);
	}
	return (1);
}

void	wait_for_turn(t_coder *coder)
{
	t_request	req;

	req.id = coder->id;
	pthread_mutex_lock(&coder->table->table_lock);
	if (strcmp(coder->rules->scheduler, "fifo") == 0)
		req.p = coder->table->ticket_dispenser++;
	else
		req.p = coder->last_compile_start + coder->rules->time_to_burnout;
	heap_push(&coder->table->heap, req);
	while (can_eat(coder, req.p) == 0 && check_death(coder->table) == 0)
		pthread_cond_wait(&coder->table->intercom, &coder->table->table_lock);
	heap_remove(&coder->table->heap, coder->id);
	coder->table->state[coder->id - 1] = 1;
	pthread_mutex_unlock(&coder->table->table_lock);
}

int	acquire_dongles(t_coder *coder)
{
	int	f;
	int	s;

	get_dongle_indices(coder, &f, &s);
	pthread_mutex_lock(&coder->dongles[f]);
	wait_cooldown(coder->table, f);
	print_status(coder, "has taken a dongle");
	pthread_mutex_lock(&coder->dongles[s]);
	wait_cooldown(coder->table, s);
	print_status(coder, "has taken a dongle");
	if (check_death(coder->table) == 1)
	{
		pthread_mutex_unlock(&coder->dongles[f]);
		pthread_mutex_unlock(&coder->dongles[s]);
		return (0);
	}
	return (1);
}

void	compile_cycle(t_coder *coder)
{
	int			f;
	int			s;
	long long	ready;

	get_dongle_indices(coder, &f, &s);
	pthread_mutex_lock(&coder->meal_lock);
	coder->last_compile_start = get_current_time(coder->table);
	coder->times_compiled++;
	print_status(coder, "is compiling");
	pthread_mutex_unlock(&coder->meal_lock);
	simulate_sleep(coder->rules->time_to_compile, coder->table);
	ready = get_current_time(coder->table) + coder->rules->dongle_cooldown;
	pthread_mutex_lock(&coder->table->time_lock);
	coder->table->dongle_ready_time[f] = ready;
	coder->table->dongle_ready_time[s] = ready;
	pthread_mutex_unlock(&coder->table->time_lock);
	pthread_mutex_unlock(&coder->table->dongles[f]);
	pthread_mutex_unlock(&coder->table->dongles[s]);
	pthread_mutex_lock(&coder->table->table_lock);
	coder->table->state[coder->id - 1] = 0;
	pthread_cond_broadcast(&coder->table->intercom);
	pthread_mutex_unlock(&coder->table->table_lock);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->rules->number_of_coders == 1)
		return (handle_single_coder(coder));
	while (check_death(coder->table) == 0)
	{
		wait_for_turn(coder);
		if (check_death(coder->table) == 1)
			break ;
		if (acquire_dongles(coder) == 0)
			break ;
		compile_cycle(coder);
		if (coder->rules->number_of_compiles_required > 0
			&& coder->times_compiled
			>= coder->rules->number_of_compiles_required)
			break ;
		post_compile_cycle(coder);
	}
	return (NULL);
}
