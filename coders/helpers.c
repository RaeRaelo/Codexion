/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: a.. <adahadda@student.1337.ma>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 13:41:38 by a..               #+#    #+#             */
/*   Updated: 2026/04/20 13:41:39 by a..              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "codexion.h"

static int	is_neighbor_dying(t_coder *me, int n_idx, long long curr_time)
{
	long long	my_clock;
	long long	th_clock;
	t_coder		*n;

	n = &me->table->coders[n_idx];
	if (n->state == 0)
		return (0);
	if (n->state == 2)
		return (1);
	my_clock = me->rules->time_to_burnout
		- (curr_time - me->last_compile_start);
	th_clock = me->rules->time_to_burnout
		- (curr_time - n->last_compile_start);
	if (th_clock < my_clock)
		return (1);
	if (th_clock == my_clock)
	{
		if (n->id % 2 == 0 && me->id % 2 != 0)
			return (1); 
		if (n->id % 2 == me->id % 2 && n->id < me->id)
			return (1);
	}
	return (0);
}

int	check_triage(t_coder *me)
{
	int			my_idx;
	int			left_idx;
	int			right_idx;
	long long	current_time;

	my_idx = me->id - 1;
	if (my_idx == 0)
		left_idx = me->rules->number_of_coders - 1;
	else
		left_idx = my_idx - 1;
	right_idx = (my_idx + 1) % me->rules->number_of_coders;
	
	current_time = get_current_time(me->table);
	if (is_neighbor_dying(me, left_idx, current_time) == 1
		|| is_neighbor_dying(me, right_idx, current_time) == 1)
		return (1);
	return (0);
}
