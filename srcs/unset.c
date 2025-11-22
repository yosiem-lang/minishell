/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 04:54:02 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 04:54:03 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// unset コマンドの実装
int	ft_unset(char **args, t_env **env)
{
	int	i;

	if (!env || !*env)
		return (1);
	if (!args || !args[1])
		return (0);
	i = 1;
	while (args[i])
	{
		if (!is_valid_identifier(args[i]))
		{
			print_error("unset", args[i], "not a valid identifier");
			i++;
			continue ;
		}
		remove_env_node(env, args[i]);
		i++;
	}
	return (0);
}
