/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: a.. <adahadda@student.1337.ma>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 12:00:00 by a                 #+#    #+#             */
/*   Updated: 2026/04/20 13:44:32 by a..              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>

typedef struct s_rules
{
	long long	number_of_coders;
	long long	time_to_burnout;
	long long	time_to_compile;
	long long	time_to_debug;
	long long	time_to_refactor;
	long long	number_of_compiles_required;
	long long	dongle_cooldown;
	char		*scheduler;
}	t_rules;

typedef struct s_coder
{
	long long		id;
	pthread_t		thread_id;
	long long		times_compiled;
	long long		last_compile_start;
	pthread_mutex_t	meal_lock;
	t_rules			*rules;
	pthread_mutex_t	*dongles;
	struct s_table	*table;
	int				state;
}	t_coder;

typedef struct s_table
{
	t_rules			*rules;
	t_coder			*coders;
	pthread_mutex_t	*dongles;
	int				simulation_dead;
	pthread_mutex_t	write_lock;
	long long		start_time;
	pthread_mutex_t	table_lock;
	long long		ticket_dispenser;
	long long		now_serving;
	pthread_cond_t	intercom;
	long long		simulated_time;
	pthread_mutex_t	time_lock;
}	t_table;

int			is_numeric_string(char *str);
int			init_rules(t_rules *rules, char **argv);
int			validate_numeric_args(char **argv);
int			validate_scheduler(char *scheduler);
void		assign_rules(t_rules *rules, char **argv);
int			validate_rules_limits(t_rules *rules);
int			init_table(t_table *table, t_rules *rules);
int			start_simulation(t_table *table);
void		*coder_routine(void *arg);
void		*monitor_routine(void *arg);
int			check_death(t_table *table);
long long	get_current_time(t_table *table);
void		print_status(t_coder *coder, char *status);
void		simulate_sleep(long long ms, t_table *table);
int			check_triage(t_coder *me);
void		wait_for_turn(t_coder *coder);
void		acquire_dongles(t_coder *coder);
void		compile_cycle(t_coder *coder);
void		post_compile_cycle(t_coder *coder);
void		check_coder_burnout(t_table *table, long long i);
void		check_completion(t_table *table, long long i,
			long long *finished_coders);
void		monitor_loop(t_table *table);

#endif
