/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 04:58:50 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 05:01:12 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// 新しい関数 1: -n オプションの処理と開始インデックスの調整 (引数3個)
// 成功時は1を返し、オプション処理を i と newline に反映
static int	handle_echo_options(char **args, int *i, int *newline)
{
	if (!args || !args[1])
		return (0);
	if (ft_strncmp(args[1], "-n", 3) == 0)
	{
		*newline = 0;
		*i = 2;
		return (1);
	}
	*i = 1;
	return (0);
}

// 新しい関数 2: 引数を出力し、スペースを挿入する (引数4個)
// 成功時は0を返す
static int	print_echo_args(char **args, t_env *env_list, int i, int newline)
{
	char	*val;

	while (args[i])
	{
		if (args[i][0] == '$' && args[i][1] != '\0')
		{
			val = get_env_value(env_list, args[i] + 1);
			if (val)
				ft_putstr_fd(val, STDOUT_FILENO);
		}
		else
		{
			expand_variables(&args[i], env_list);
			ft_putstr_fd(args[i], STDOUT_FILENO);
		}
		if (args[i + 1])
			ft_putchar_fd(' ', STDOUT_FILENO);
		i++;
	}
	if (newline)
		ft_putchar_fd('\n', STDOUT_FILENO);
	return (0);
}

// メイン関数 (引数2個: args, env)
int	ft_echo(char **args, t_env **env)
{
	int	i;
	int	newline;

	if (!args || !args[1])
	{
		ft_putchar_fd('\n', STDOUT_FILENO);
		return (0);
	}
	newline = 1;
	handle_echo_options(args, &i, &newline);
	return (print_echo_args(args, *env, i, newline));
}
