/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 04:54:44 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/23 21:11:47 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// exit コマンドの実装
int	ft_exit(char **args, t_env **env)
{
	int	exit_code;

	(void)env;
	ft_putendl_fd("exit", STDOUT_FILENO);
	if (!args || !args[1])
		exit_code = 0;
	else
	{
		if (args[2])
		{
			print_error("exit", NULL, "too many arguments");
			return (1);
		}
		if (!ft_strisdigit(args[1]))
		{
			print_error("exit", args[1], "numeric argument required");
			exit_code = 255;
		}
		else
			exit_code = ft_atoi(args[1]);
	}
	exit(exit_code);
}
