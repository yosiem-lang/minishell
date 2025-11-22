/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_args.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 05:05:00 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 05:05:00 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	**tokens_to_array(t_token *tok)
{
	char	**array;
	int		count;
	t_token	*current;

	count = 0;
	current = tok;
	while (current && current->kind != TK_EOF)
	{
		count++;
		current = current->next;
	}
	if (count == 0)
		return (NULL);
	array = malloc(sizeof(char *) * (count + 1));
	if (!array)
		return (NULL);
	if (fill_token_array(tok, array, count) != 0)
	{
		free(array);
		return (NULL);
	}
	return (array);
}

int	fill_token_array(t_token *tok, char **array, int count)
{
	int		i;
	t_token	*current;

	i = 0;
	current = tok;
	while (current && current->kind != TK_EOF)
	{
		array[i] = ft_strdup(current->word);
		if (!array[i])
		{
			while (i > 0)
				free(array[--i]);
			return (1);
		}
		i++;
		current = current->next;
	}
	array[i] = NULL;
	if (i != count)
	{
		while (i > 0)
			free(array[--i]);
		return (1);
	}
	return (0);
}

char	**extract_args(char **tokens)
{
	char	**args;
	int		count;

	count = count_command_args(tokens);
	if (count == 0)
		return (NULL);
	args = malloc(sizeof(char *) * (count + 1));
	if (!args)
		return (NULL);
	if (fill_command_args(tokens, args, count) != 0)
	{
		free(args);
		return (NULL);
	}
	return (args);
}

int	count_command_args(char **tokens)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (tokens[i])
	{
		if (is_redirection(tokens[i]))
		{
			i += 2;
			continue ;
		}
		if (ft_strncmp(tokens[i], "|", 2) == 0)
			break ;
		count++;
		i++;
	}
	return (count);
}
