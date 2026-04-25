/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: a.. <adahadda@student.1337.ma>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 12:00:00 by a                 #+#    #+#             */
/*   Updated: 2026/04/25 01:42:33 by a..              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	get_top(t_heap *h, int l, int r)
{
	if (r < h->size)
	{
		if (h->data[r].p < h->data[l].p)
			return (r);
		if (h->data[r].p == h->data[l].p
			&& h->data[r].id > h->data[l].id)
			return (r);
	}
	return (l);
}

static void	heap_bubble_up(t_heap *h, int i)
{
	t_request	t;
	int			p;

	while (i != 0 && (h->data[(i - 1) / 2].p > h->data[i].p
			|| (h->data[(i - 1) / 2].p == h->data[i].p
				&& h->data[(i - 1) / 2].id < h->data[i].id)))
	{
		p = (i - 1) / 2;
		t = h->data[i];
		h->data[i] = h->data[p];
		h->data[p] = t;
		i = p;
	}
}

static void	heap_bubble_down(t_heap *h, int i)
{
	int			s;
	t_request	t;

	while (2 * i + 1 < h->size)
	{
		s = get_top(h, 2 * i + 1, 2 * i + 2);
		if (h->data[i].p < h->data[s].p
			|| (h->data[i].p == h->data[s].p
				&& h->data[i].id > h->data[s].id))
			break ;
		t = h->data[i];
		h->data[i] = h->data[s];
		h->data[s] = t;
		i = s;
	}
}

void	heap_push(t_heap *h, t_request r)
{
	int	i;
	int	p;

	i = h->size++;
	h->data[i] = r;
	while (i != 0)
	{
		p = (i - 1) / 2;
		if (h->data[p].p < h->data[i].p || (h->data[p].p == h->data[i].p
				&& h->data[p].id > h->data[i].id))
			break ;
		r = h->data[i];
		h->data[i] = h->data[p];
		h->data[p] = r;
		i = p;
	}
}

void	heap_remove(t_heap *h, int id)
{
	int	i;

	i = -1;
	while (++i < h->size)
		if (h->data[i].id == id)
			break ;
	if (i == h->size)
		return ;
	h->data[i] = h->data[--h->size];
	heap_bubble_up(h, i);
	heap_bubble_down(h, i);
}
