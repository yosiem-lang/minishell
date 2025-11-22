/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    ft +:+         +:+      */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 05:25:00 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 05:25:00 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_absolute_or_relative_path(char *cmd)
{
	if (!cmd)
		return (0);
	if (cmd[0] == '/')
		return (1);
	if (ft_strchr(cmd, '/'))
		return (1);
	return (0);
}

static char	*find_absolute_or_relative(char *cmd)
{
	if (access(cmd, F_OK) == 0)
		return (ft_strdup(cmd));
	return (NULL);
}

static char	*find_in_path(char *cmd, t_env *env)
{
	char	*path_value;
	char	**path_dirs;
	char	*full_path;

	path_value = get_env_value(env, "PATH");
	if (!path_value)
		return (NULL);
	path_dirs = ft_split(path_value, ':');
	if (!path_dirs)
		return (NULL);
	full_path = search_path_dirs(path_dirs, cmd);
	if (full_path)
		return (full_path);
	free_array(path_dirs);
	return (NULL);
}

char	*find_executable(char *cmd, t_env *env)
{
	char	*exec_path;

	if (!cmd || !env)
		return (NULL);
	if (is_absolute_or_relative_path(cmd))
		return (find_absolute_or_relative(cmd));
	exec_path = find_in_path(cmd, env);
	return (exec_path);
}
