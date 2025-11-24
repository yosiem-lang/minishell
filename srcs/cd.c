/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 04:58:44 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/23 18:13:00 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	update_pwd_env(t_env **env, char *old_pwd, char *path)
{
	char	*new_pwd;

	new_pwd = getcwd(NULL, 0);
	if (!new_pwd)
	{
		print_error("cd", path, "getcwd failed");
		free(old_pwd);
		return (1);
	}
	if (get_env_value(*env, "OLDPWD") == NULL)
		add_env_node(env, "OLDPWD", old_pwd);
	else
		update_env_value(*env, "OLDPWD", old_pwd);
	if (get_env_value(*env, "PWD") == NULL)
		add_env_node(env, "PWD", new_pwd);
	else
		update_env_value(*env, "PWD", new_pwd);
	free(old_pwd);
	free(new_pwd);
	return (0);
}

static char	*get_cd_path(char **args, t_env *env_list)
{
	char	*path;

	if (!args || !args[1])
	{
		path = get_env_value(env_list, "HOME");
		if (!path)
		{
			print_error("cd", NULL, "HOME not set");
			return (NULL);
		}
	}
	else
		path = args[1];
	return (path);
}

static int	execute_cd(char *path, t_env **env)
{
	char	*old_pwd;
	int		status;

	old_pwd = getcwd(NULL, 0);
	if (!old_pwd)
		old_pwd = ft_strdup("");
	if (chdir(path) != 0)
	{
		print_error("cd", path, strerror(errno));
		free(old_pwd);
		return (1);
	}
	status = update_pwd_env(env, old_pwd, path);
	return (status);
}

int	ft_cd(char **args, t_env **env)
{
	char	*path;

	if (!env || !*env)
		return (1);
	path = get_cd_path(args, *env);
	if (!path)
		return (1);
	return (execute_cd(path, env));
}
