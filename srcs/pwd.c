/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 04:54:16 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 04:54:17 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// pwd コマンドの実装
int	ft_pwd(char **args, t_env **env)
{
	char	*cwd;

	(void)args;
	(void)env;
	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		print_error("pwd", NULL, strerror(errno));
		return (1);
	}
	ft_putendl_fd(cwd, STDOUT_FILENO);
	free(cwd);
	return (0);
}
