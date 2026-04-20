/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rules.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: a.. <adahadda@student.1337.ma>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 12:00:00 by a                 #+#    #+#             */
/*   Updated: 2026/04/20 13:43:20 by a..              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <string.h>
#include "codexion.h"


// validate_numeric_args: Checks if first 7 args are numeric
int	validate_numeric_args(char **argv)
{
	int	i;

	i = 1;
	while (i <= 7)
	{
		if (is_numeric_string(argv[i]) == 0)
			return (1);
		i++;
	}
	return (0);
}

// validate_scheduler: Checks if scheduler is "fifo" or "edf"
int	validate_scheduler(char *scheduler)
{
	if (strcmp(scheduler, "fifo") != 0 && strcmp(scheduler, "edf") != 0)
		return (1);
	return (0);
}

// assign_rules: Assigns values from argv to rules struct
void	assign_rules(t_rules *rules, char **argv)
{
	rules->number_of_coders = atoll(argv[1]);
	rules->time_to_burnout = atoll(argv[2]);
	rules->time_to_compile = atoll(argv[3]);
	rules->time_to_debug = atoll(argv[4]);
	rules->time_to_refactor = atoll(argv[5]);
	rules->number_of_compiles_required = atoll(argv[6]);
	rules->dongle_cooldown = atoll(argv[7]);
	rules->scheduler = argv[8];
}

// validate_rules_limits: Checks if values exceed INT_MAX
int	validate_rules_limits(t_rules *rules)
{
	if (rules->number_of_coders > 2147483647LL
		|| rules->time_to_burnout > 2147483647LL
		|| rules->time_to_compile > 2147483647LL
		|| rules->time_to_debug > 2147483647LL
		|| rules->time_to_refactor > 2147483647LL
		|| rules->number_of_compiles_required > 2147483647LL
		|| rules->dongle_cooldown > 2147483647LL)
		return (1);
	return (0);
}

// init_rules: Parses command-line arguments into t_rules structure
int	init_rules(t_rules *rules, char **argv)
{
	if (validate_numeric_args(argv) == 1)
		return (1);
	if (validate_scheduler(argv[8]) == 1)
		return (1);
	assign_rules(rules, argv);
	if (validate_rules_limits(rules) == 1)
		return (1);
	return (0);
}
