/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_pipe.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 05:05:00 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 05:05:00 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	copy_and_cleanup_arg(char **tokens,
		char **args, int *current_j_ptr, int token_idx)
{
	int	current_j;

	current_j = *current_j_ptr;
	args[current_j] = ft_strdup(tokens[token_idx]);
	if (!args[current_j])
	{
		while (current_j > 0)
			free(args[--current_j]);
		return (1);
	}
	*current_j_ptr = current_j + 1;
	return (0);
}

char	**get_pipe_command(char **tokens, int *start_idx)
{
	char	**args;
	int		count;

	count = count_command_args_in_pipe(tokens, *start_idx);
	if (count == 0)
		return (NULL);
	args = malloc(sizeof(char *) * (count + 1));
	if (!args)
		return (NULL);
	if (fill_pipe_command_args(tokens, args, start_idx, count) != 0)
	{
		free(args);
		return (NULL);
	}
	return (args);
}

int	count_command_args_in_pipe(char **tokens, int start_i)
{
	int	i;
	int	count;

	i = start_i;
	count = 0;
	while (tokens[i] && ft_strncmp(tokens[i], "|", 2) != 0)
	{
		if (is_redirection(tokens[i]))
		{
			i += 2;
			continue ;
		}
		count++;
		i++;
	}
	return (count);
}

int	fill_pipe_command_args(char **tokens,
		char **args, int *start_idx_ptr, int count)
{
	int	i;
	int	j;

	(void)count;
	i = *start_idx_ptr;
	j = 0;
	while (tokens[i] && ft_strncmp(tokens[i], "|", 2) != 0)
	{
		if (is_redirection(tokens[i]))
		{
			i += 2;
			continue ;
		}
		if (copy_and_cleanup_arg(tokens, args, &j, i) != 0)
			return (1);
		i++;
	}
	args[j] = NULL;
	if (tokens[i] && ft_strncmp(tokens[i], "|", 2) == 0)
		*start_idx_ptr = i + 1;
	else
		*start_idx_ptr = i;
	return (0);
}
