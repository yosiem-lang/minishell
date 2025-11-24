/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 03:59:24 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/25 03:34:12 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*-----------------------------------------------------------
** Utility functions
-----------------------------------------------------------*/

void	fatal_error(const char *msg)
{
	ft_putstr_fd("Fatal Error: ", STDERR_FILENO);
	ft_putendl_fd((char *)msg, STDERR_FILENO);
	exit(1);
}

void	assert_error(const char *msg)
{
	ft_putstr_fd("Assert Error: ", STDERR_FILENO);
	ft_putendl_fd((char *)msg, STDERR_FILENO);
	exit(255);
}

/*-----------------------------------------------------------
** Token constructor
-----------------------------------------------------------*/

t_token	*new_token(char *word, t_token_kind kind)
{
	t_token	*tok;

	tok = ft_calloc(1, sizeof(*tok));
	if (tok == NULL)
		fatal_error("ft_calloc");
	tok->word = word;
	tok->kind = kind;
	tok->next = NULL;
	return (tok);
}

bool	is_blank(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

bool	consume_blank(char **rest, char *line)
{
	while (*line && is_blank(*line))
		line++;
	*rest = line;
	return (*line != '\0');
}

// /*-----------------------------------------------------------
// ** main for testing
// -----------------------------------------------------------*/

// int	main(void)
// {
// 	char line[256];

// 	while (1)
// 	{
// 		printf("minishell$ ");
// 		if (!fgets(line, sizeof(line), stdin))
// 			break;
// 		t_token *tok = tokenize(line);
// 		print_tokens(tok);
// 		free_token(tok);
// 	}
// 	return (0);
// }
