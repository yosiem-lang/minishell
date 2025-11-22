/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_list.c                                      :+:      :+:    :+:   */
/*                                                    ft +:+         +:+      */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 05:25:00 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 05:25:00 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	extract_key_value(char *arg, char **key_out, char **value_out)
{
	char	*equal_pos;

	equal_pos = ft_strchr(arg, '=');
	if (equal_pos)
	{
		*equal_pos = '\0';
		*key_out = arg;
		*value_out = equal_pos + 1;
	}
	else
	{
		*key_out = arg;
		*value_out = NULL;
	}
}

int	print_export_list(t_env *env)
{
	t_env	*current;

	if (!env)
		return (1);
	current = env;
	while (current)
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		ft_putstr_fd(current->key, STDOUT_FILENO);
		if (current->value)
		{
			ft_putstr_fd("=\"", STDOUT_FILENO);
			ft_putstr_fd(current->value, STDOUT_FILENO);
			ft_putstr_fd("\"", STDOUT_FILENO);
		}
		ft_putchar_fd('\n', STDOUT_FILENO);
		current = current->next;
	}
	return (0);
}
