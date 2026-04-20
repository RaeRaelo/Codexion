/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: a.. <adahadda@student.1337.ma>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 12:00:00 by a                 #+#    #+#             */
/*   Updated: 2026/04/20 13:41:31 by a..              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "codexion.h"


// wait_for_turn: Waits for the coder's turn based on scheduler
void	wait_for_turn(t_coder *coder)
{
	long long	my_ticket;

	if (strcmp(coder->rules->scheduler, "fifo") == 0)
	{
		pthread_mutex_lock(&coder->table->table_lock);
		my_ticket = coder->table->ticket_dispenser;
		coder->table->ticket_dispenser++;
		while (my_ticket != coder->table->now_serving
			&& check_death(coder->table) == 0)
			pthread_cond_wait(&coder->table->intercom,
				&coder->table->table_lock);
		pthread_mutex_unlock(&coder->table->table_lock);
	}
	else if (strcmp(coder->rules->scheduler, "edf") == 0)
	{
		pthread_mutex_lock(&coder->table->table_lock);
		coder->state = 1;
		while (check_triage(coder) == 1
			&& check_death(coder->table) == 0)
			pthread_cond_wait(&coder->table->intercom,
				&coder->table->table_lock);
		coder->state = 2;
		pthread_mutex_unlock(&coder->table->table_lock);
	}
}

// acquire_dongles: Acquires the two dongles for the coder
void	acquire_dongles(t_coder *coder)
{
	pthread_mutex_t	*left_dongle;
	pthread_mutex_t	*right_dongle;

	left_dongle = &coder->dongles[coder->id - 1];
	right_dongle = &coder->dongles[coder->id
		% coder->rules->number_of_coders];
	if (coder->id % 2 == 0)
	{
		pthread_mutex_lock(left_dongle);
		print_status(coder, "has taken a dongle");
		pthread_mutex_lock(right_dongle);
		print_status(coder, "has taken a dongle");
	}
	else
	{
		pthread_mutex_lock(right_dongle);
		print_status(coder, "has taken a dongle");
		pthread_mutex_lock(left_dongle);
		print_status(coder, "has taken a dongle");
	}
}

// compile_cycle: Performs the compilation cycle
void	compile_cycle(t_coder *coder)
{
	pthread_mutex_t	*left_dongle;
	pthread_mutex_t	*right_dongle;

	left_dongle = &coder->dongles[coder->id - 1];
	right_dongle = &coder->dongles[coder->id
		% coder->rules->number_of_coders];
	pthread_mutex_lock(&coder->meal_lock);
	coder->last_compile_start = get_current_time(coder->table);
	print_status(coder, "is compiling");
	simulate_sleep(coder->rules->time_to_compile,
		coder->table);
	coder->times_compiled++;
	pthread_mutex_unlock(&coder->meal_lock);
	pthread_mutex_unlock(left_dongle);
	pthread_mutex_unlock(right_dongle);
	pthread_mutex_lock(&coder->table->table_lock);
	coder->state = 0;
	coder->table->now_serving++;
	pthread_cond_broadcast(&coder->table->intercom);
	pthread_mutex_unlock(&coder->table->table_lock);
}

// post_compile_cycle: Performs debug and refactor cycles
void	post_compile_cycle(t_coder *coder)
{
	print_status(coder, "is debugging");
	simulate_sleep(coder->rules->time_to_debug,
		coder->table);
	print_status(coder, "is refactoring");
	simulate_sleep(coder->rules->time_to_refactor,
		coder->table);
	if (coder->rules->dongle_cooldown > 0)
		simulate_sleep(coder->rules->dongle_cooldown,
			coder->table);
}

// coder_routine: Main thread routine for each coder
void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->rules->number_of_coders == 1)
	{
		pthread_mutex_lock(&coder->dongles[0]);
		print_status(coder, "has taken a dongle");
		simulate_sleep(coder->rules->time_to_burnout, coder->table);
		pthread_mutex_unlock(&coder->dongles[0]);
		return (NULL);
	}
	while (check_death(coder->table) == 0)
	{
		wait_for_turn(coder);
		if (check_death(coder->table) == 1)
			break;
		acquire_dongles(coder);
		compile_cycle(coder);
		if (coder->rules->number_of_compiles_required > 0
			&& coder->times_compiled
			>= coder->rules->number_of_compiles_required)
			return (NULL);
		post_compile_cycle(coder);
	}
	return (NULL);
}
