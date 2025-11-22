/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_heredoc.c                                 :+:      :+:    :+:   */
/*                                                    ft +:+         +:+      */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 05:25:00 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 05:25:00 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	heredoc_setup(int *pipe_fd_ptr)
{
	if (pipe(pipe_fd_ptr) < 0)
	{
		print_error("pipe", NULL, strerror(errno));
		return (-1);
	}
	return (0);
}

static int	heredoc_input_loop(char *delimiter, int write_fd)
{
	char	*line;
	char	*prompt;

	if (!delimiter || write_fd < 0)
		return (1);
	prompt = "> ";
	while (1)
	{
		line = readline(prompt);
		if (!line)
		{
			ft_putendl_fd("", STDOUT_FILENO);
			break ;
		}
		if (ft_strncmp(line, delimiter, ft_strlen(delimiter) + 1) == 0)
		{
			free(line);
			break ;
		}
		ft_putendl_fd(line, write_fd);
		free(line);
	}
	close(write_fd);
	return (0);
}

int	handle_heredoc(char *delimiter)
{
	int	pipe_fd[2];

	if (!delimiter)
		return (-1);
	if (heredoc_setup(pipe_fd) < 0)
		return (-1);
	if (heredoc_input_loop(delimiter, pipe_fd[1]) != 0)
	{
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		return (-1);
	}
	return (pipe_fd[0]);
}
