/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cat_options.c                                      :+:      :+:    :+:   */
/*                                                    ft +:+         +:+      */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 05:25:00 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 05:25:00 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	set_options_from_arg(char *arg, t_cat_options *opts)
{
	int	j;

	j = 1;
	while (arg[j])
	{
		if (arg[j] == 'e' || arg[j] == 'E')
			opts->show_ends = 1;
		else if (arg[j] == 'n')
			opts->number_lines = 1;
		else if (arg[j] == 'b')
			opts->number_nonblank = 1;
		else if (arg[j] == 's')
			opts->squeeze_blank = 1;
		else
			return (0);
		j++;
	}
	return (1);
}

int	parse_options(char **args, t_cat_options *opts, int *arg_start)
{
	int	i;

	*opts = (t_cat_options){0, 0, 0, 0};
	*arg_start = 1;
	i = 1;
	while (args[i] && args[i][0] == '-')
	{
		if (ft_strlen(args[i]) == 1)
			break ;
		if (set_options_from_arg(args[i], opts) == 0)
			return (0);
		i++;
		(*arg_start)++;
	}
	if (opts->number_nonblank)
		opts->number_lines = 0;
	return (1);
}
