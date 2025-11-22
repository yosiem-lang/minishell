/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fill_args.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 05:05:00 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 05:05:00 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	copy_and_check_arg(char **tokens,
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

static int	check_and_cleanup_final(char **args, int j, int count)
{
	if (j != count)
	{
		while (j > 0)
			free(args[--j]);
		return (1);
	}
	return (0);
}

int	fill_command_args(char **tokens, char **args, int count)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (tokens[i] && j < count)
	{
		if (is_redirection(tokens[i]))
		{
			i += 2;
			continue ;
		}
		if (ft_strncmp(tokens[i], "|", 2) == 0)
			break ;
		if (copy_and_check_arg(tokens, args, &j, i) != 0)
			return (1);
		i++;
	}
	args[j] = NULL;
	return (check_and_cleanup_final(args, j, count));
}

int	is_redirection(char *token)
{
	return (ft_strncmp(token, "<", 2) == 0
		|| ft_strncmp(token, ">", 2) == 0
		|| ft_strncmp(token, ">>", 3) == 0
		|| ft_strncmp(token, "<<", 3) == 0);
}

int	has_pipe(char **tokens)
{
	int	i;

	i = 0;
	while (tokens[i])
	{
		if (ft_strncmp(tokens[i], "|", 2) == 0)
			return (1);
		i++;
	}
	return (0);
}
