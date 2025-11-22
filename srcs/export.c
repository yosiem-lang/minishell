/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    ft +:+         +:+      */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 04:54:39 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 05:25:00 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	handle_export_argument(char *arg, t_env **env);
static int	process_env_update(t_env **env, char *key, char *value,
				char *equal_pos);

int	ft_export(char **args, t_env **env)
{
	int	i;
	int	status;

	if (!env || !*env)
		return (1);
	if (!args || !args[1])
		return (print_export_list(*env));
	i = 1;
	status = 0;
	while (args[i])
	{
		if (handle_export_argument(args[i], env) != 0)
			status = 1;
		i++;
	}
	return (status);
}

static int	handle_export_argument(char *arg, t_env **env)
{
	char	*key;
	char	*value;
	char	*equal_pos;
	int		status;

	equal_pos = ft_strchr(arg, '=');
	if (equal_pos)
	{
		*equal_pos = '\0';
		key = arg;
		value = equal_pos + 1;
	}
	else
	{
		key = arg;
		value = NULL;
	}
	status = process_env_update(env, key, value, equal_pos);
	return (status);
}

static int	process_env_update(t_env **env, char *key,
			char *value, char *equal_pos)
{
	if (!is_valid_identifier(key))
	{
		if (equal_pos)
			*equal_pos = '=';
		print_error("export", key, "not a valid identifier");
		return (1);
	}
	if (get_env_value(*env, key))
		update_env_value(*env, key, value);
	else
		add_env_node(env, key, value);
	if (equal_pos)
		*equal_pos = '=';
	return (0);
}
