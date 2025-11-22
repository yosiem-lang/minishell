/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_exec.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 05:05:00 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 05:05:00 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	cleanup_parent_fds_and_pipe(t_fd_info *fdi_ptr, int has_next)
{
	if (fdi_ptr->in_fd >= 0)
		close(fdi_ptr->in_fd);
	if (fdi_ptr->out_fd >= 0)
		close(fdi_ptr->out_fd);
	if (fdi_ptr->prev_pipe_read >= 0)
		close(fdi_ptr->prev_pipe_read);
	if (has_next)
	{
		close(fdi_ptr->pipe_fd[1]);
		fdi_ptr->prev_pipe_read = fdi_ptr->pipe_fd[0];
	}
	else
		fdi_ptr->prev_pipe_read = -1;
}

static int	setup_pipe_if_needed(t_fd_info *fdi_ptr, int has_next)
{
	if (has_next)
	{
		if (pipe(fdi_ptr->pipe_fd) < 0)
		{
			print_error("pipe", NULL, strerror(errno));
			return (-1);
		}
	}
	return (0);
}

static pid_t	execute_segment_process(char **args,
		t_env **env, t_fd_info *fdi_ptr, int has_next)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
		execute_child_process_pipe(args, env, *fdi_ptr, has_next);
	cleanup_parent_fds_and_pipe(fdi_ptr, has_next);
	if (!has_next)
	{
		waitpid(pid, &fdi_ptr->status, 0);
		free_array(args);
		return (pid);
	}
	free_array(args);
	return (pid);
}

pid_t	pipe_loop_segment(char **tokens,
		t_env **env, t_fd_info *fdi_ptr, int *start_idx_ptr)
{
	char			**args;
	t_token_range	range;
	int				has_next;

	find_pipe_segment(tokens, start_idx_ptr, &range, &has_next);
	if (setup_pipe_if_needed(fdi_ptr, has_next) != 0)
		return (-1);
	setup_pipe_redirections(tokens, range, &fdi_ptr->in_fd, &fdi_ptr->out_fd);
	args = get_pipe_command(tokens, start_idx_ptr);
	if (!args || !args[0])
	{
		if (has_next)
		{
			close(fdi_ptr->pipe_fd[0]);
			close(fdi_ptr->pipe_fd[1]);
		}
		return (0);
	}
	return (execute_segment_process(args, env, fdi_ptr, has_next));
}

int	execute_pipe(char **tokens, t_env **env)
{
	t_fd_info		fdi;
	int				status;
	int				start_idx;
	pid_t			last_pid;

	fdi.prev_pipe_read = -1;
	start_idx = 0;
	while (1)
	{
		last_pid = pipe_loop_segment(tokens, env, &fdi, &start_idx);
		if (last_pid == 0)
			break ;
		if (last_pid == -1)
			return (1);
		if (fdi.prev_pipe_read == -1)
			break ;
	}
	while (waitpid(-1, &status, 0) > 0)
		;
	if (WIFEXITED(fdi.status))
		return (WEXITSTATUS(fdi.status));
	return (1);
}
