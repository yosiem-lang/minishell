/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 04:58:34 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 05:02:08 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_builtin	*get_builtins(void)
{
	static t_builtin	builtins[] = {
	{"echo", ft_echo},
	{"cd", ft_cd},
	{"pwd", ft_pwd},
	{"export", ft_export},
	{"unset", ft_unset},
	{"env", ft_env},
	{"exit", ft_exit},
	{"cat", ft_cat},
	{NULL, NULL}
	};

	return (builtins);
}

// ビルトインコマンドかどうかをチェック
int	is_builtin(char *cmd)
{
	t_builtin	*builtins;
	int			i;

	if (!cmd)
		return (0);
	builtins = get_builtins();
	i = 0;
	while (builtins[i].name)
	{
		if (ft_strncmp(cmd, builtins[i].name,
				ft_strlen(builtins[i].name) + 1) == 0)
			return (1);
		i++;
	}
	return (0);
}

// ビルトインコマンドを実行（ディスパッチ）
int	execute_builtin(char **args, t_env **env)
{
	t_builtin	*builtins;
	int			i;

	if (!args || !args[0])
		return (1);
	builtins = get_builtins();
	i = 0;
	while (builtins[i].name)
	{
		if (ft_strncmp(args[0], builtins[i].name,
				ft_strlen(builtins[i].name) + 1) == 0)
			return (builtins[i].func(args, env));
		i++;
	}
	return (1);
}
