/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_setup.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 05:25:00 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 05:25:00 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	setup_handle_heredoc_redirection(char **tokens,
		int *in_fd, int *current_idx_ptr)
{
	int	fd;
	int	i;

	i = *current_idx_ptr;
	if (ft_strncmp(tokens[i], "<<", 3) == 0 && ft_strlen(tokens[i]) == 2)
	{
		if (!tokens[i + 1])
		{
			print_error("syntax error", NULL, "unexpected end of file");
			return (-1);
		}
		if (*in_fd >= 0)
			close(*in_fd);
		fd = handle_heredoc(tokens[i + 1]);
		if (fd < 0)
			return (-1);
		*in_fd = fd;
		*current_idx_ptr += 2;
		return (1);
	}
	return (0);
}

static int	setup_handle_file_input_redirection(char **tokens,
		int *in_fd, int *current_idx_ptr)
{
	int	fd;
	int	i;

	i = *current_idx_ptr;
	if (ft_strncmp(tokens[i], "<", 2) == 0 && ft_strlen(tokens[i]) == 1)
	{
		if (!tokens[i + 1])
		{
			print_error("syntax error", NULL, "unexpected end of file");
			return (-1);
		}
		if (*in_fd >= 0)
			close(*in_fd);
		fd = open_input_file(tokens[i + 1]);
		if (fd < 0)
			return (-1);
		*in_fd = fd;
		*current_idx_ptr += 2;
		return (1);
	}
	return (0);
}

static int	setup_handle_input_redirections(char **tokens,
		int *in_fd, int *current_idx_ptr)
{
	int	ret;

	ret = setup_handle_heredoc_redirection(tokens, in_fd, current_idx_ptr);
	if (ret != 0)
		return (ret);
	ret = setup_handle_file_input_redirection(tokens, in_fd, current_idx_ptr);
	return (ret);
}

static int	setup_handle_output_redirections(char **tokens,
		int *out_fd, int *current_idx_ptr)
{
	int	fd;
	int	append_mode;
	int	i;

	i = *current_idx_ptr;
	if ((ft_strncmp(tokens[i], ">>", 3) == 0 && ft_strlen(tokens[i]) == 2)
		|| (ft_strncmp(tokens[i], ">", 2) == 0 && ft_strlen(tokens[i]) == 1))
	{
		if (!tokens[i + 1])
		{
			print_error("syntax error", NULL, "unexpected end of file");
			return (-1);
		}
		if (*out_fd >= 0)
			close(*out_fd);
		append_mode = (ft_strncmp(tokens[i], ">>", 3) == 0);
		fd = open_output_file(tokens[i + 1], append_mode);
		if (fd < 0)
			return (-1);
		*out_fd = fd;
		*current_idx_ptr += 2;
		return (1);
	}
	return (0);
}

int	setup_redirections(char **tokens, int *in_fd, int *out_fd)
{
	int	i;
	int	ret;

	if (!tokens || !in_fd || !out_fd)
		return (1);
	*in_fd = -1;
	*out_fd = -1;
	i = 0;
	while (tokens[i])
	{
		ret = setup_handle_input_redirections(tokens, in_fd, &i);
		if (ret == -1)
			return (1);
		if (ret == 1)
			continue ;
		ret = setup_handle_output_redirections(tokens, out_fd, &i);
		if (ret == -1)
			return (1);
		if (ret == 1)
			continue ;
		i++;
	}
	return (0);
}
