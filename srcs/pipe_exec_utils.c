/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_exec_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 05:42:55 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/23 04:17:48 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	find_pipe_segment(char **tokens,
		int *start_idx_ptr, t_token_range *range_ptr, int *has_next_ptr)
{
	int	end_idx;

	range_ptr->start_idx = *start_idx_ptr;
	end_idx = *start_idx_ptr;
	while (tokens[end_idx] && ft_strncmp(tokens[end_idx], "|", 2) != 0)
		end_idx++;
	range_ptr->end_idx = end_idx;
	*has_next_ptr = (tokens[end_idx] != NULL);
}

static void	setup_input_redirection(t_fd_info fd_info)
{
	if (fd_info.in_fd >= 0)
	{
		dup2(fd_info.in_fd, STDIN_FILENO);
		close(fd_info.in_fd);
	}
	else if (fd_info.prev_pipe_read >= 0)
	{
		dup2(fd_info.prev_pipe_read, STDIN_FILENO);
		close(fd_info.prev_pipe_read);
	}
}

static void	setup_output_redirection(t_fd_info fd_info, int has_next)
{
	if (fd_info.out_fd >= 0)
	{
		dup2(fd_info.out_fd, STDOUT_FILENO);
		close(fd_info.out_fd);
	}
	else if (has_next)
	{
		dup2(fd_info.pipe_fd[1], STDOUT_FILENO);
		close(fd_info.pipe_fd[0]);
		close(fd_info.pipe_fd[1]);
	}
}

void	execute_child_process_pipe(char **args,
		t_env **env, t_fd_info fd_info, int has_next)
{
	child_signal_setting();
	setup_input_redirection(fd_info);
	setup_output_redirection(fd_info, has_next);
	if (is_builtin(args[0]))
		exit(execute_builtin(args, env));
	else
		exit(execute_external(args, env));
}
