/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 05:43:19 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 05:48:54 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*join_path(const char *dir, const char *cmd)
{
	char	*tmp;
	char	*full;

	tmp = ft_strjoin(dir, "/");
	if (!tmp)
		return (NULL);
	full = ft_strjoin(tmp, cmd);
	free(tmp);
	return (full);
}

char	*search_path_dirs(char **path_dirs, char *cmd)
{
	char	*full_path;
	int		i;

	i = 0;
	while (path_dirs[i])
	{
		full_path = join_path(path_dirs[i], cmd);
		if (full_path && access(full_path, F_OK) == 0)
		{
			free_array(path_dirs);
			return (full_path);
		}
		if (full_path)
			free(full_path);
		i++;
	}
	return (NULL);
}
