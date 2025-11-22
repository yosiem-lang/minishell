/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 04:54:34 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 05:07:52 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// メイン関数
int	main(int argc, char **argv, char **envp)
{
	t_env	*env;

	(void)argc;
	(void)argv;
	setup_signal_handlers();
	env = init_env(envp);
	if (!env)
	{
		ft_putendl_fd("minishell: failed to initialize environment",
			STDERR_FILENO);
		return (1);
	}
	minishell_loop(&env);
	free_env_list(env);
	return (0);
}

// メインループ
int	minishell_loop(t_env **env)
{
	char	*input;
	char	*prompt;

	prompt = "minishell$ ";
	while (1)
	{
		input = readline(prompt);
		if (!input)
		{
			ft_putendl_fd("exit", STDOUT_FILENO);
			break ;
		}
		if (ft_strlen(input) > 0)
			add_history(input);
		parse_and_execute(input, env);
		free(input);
	}
	return (0);
}

void	parse_and_execute(char *input, t_env **env)
{
	char	**tokens;
	char	**args;
	t_fds	fds;
	int		ret;

	tokens = NULL;
	args = NULL;
	if (!input || ft_strlen(input) == 0)
		return ;
	ret = handle_tokenization_and_args(input, env, &tokens, &args);
	if (ret == -1)
		return ;
	if (ret == 1)
	{
		g_signal = execute_pipe(tokens, env);
		free_array(tokens);
		return ;
	}
	execute_single_command(tokens, args, env, &fds);
}
