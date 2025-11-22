/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cat.c                                              :+:      :+:    :+:   */
/*                                                    ft +:+         +:+      */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 04:58:24 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 05:25:00 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_cat(char **args, t_env **env)
{
	int				i;
	int				exit_status;
	int				arg_start;
	t_cat_options	opts;

	(void)env;
	if (!args)
		return (1);
	if (!parse_options(args, &opts, &arg_start))
	{
		print_error("cat", args[1], "invalid option");
		return (1);
	}
	if (!args[arg_start])
		return (read_from_stdin(&opts));
	exit_status = 0;
	i = arg_start;
	while (args[i])
	{
		if (read_and_print_file(args[i], &opts) != 0)
			exit_status = 1;
		i++;
	}
	return (exit_status);
}
