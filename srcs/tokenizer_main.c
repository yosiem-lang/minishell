/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_main.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 05:05:00 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/23 04:09:15 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_token	*tokenize_dispatcher(t_token *current_tail,
		char **line_ptr, char *line)
{
	t_token	*new_tok;

	if (is_blank(*line))
	{
		consume_blank(line_ptr, line);
		return (NULL);
	}
	else if (is_operator(line))
		new_tok = operator(line_ptr, line);
	else if (is_word(line))
		new_tok = word(line_ptr, line);
	else
	{
		assert_error("Unexpected Token");
		return (NULL);
	}
	current_tail->next = new_tok;
	return (new_tok);
}

t_token	*tokenize(char *line)
{
	t_token	head;
	t_token	*tok;
	t_token	*new_tail;

	head.next = NULL;
	tok = &head;
	while (*line)
	{
		new_tail = tokenize_dispatcher(tok, &line, line);
		if (new_tail != NULL)
			tok = new_tail;
	}
	tok->next = new_token(NULL, TK_EOF);
	return (head.next);
}

void	print_tokens(t_token *tok)
{
	while (tok && tok->kind != TK_EOF)
	{
		printf("[%s]", tok->word);
		if (tok->kind == TK_OP)
			printf(" (OP)");
		printf("\n");
		tok = tok->next;
	}
	printf("(EOF)\n");
}

void	free_token(t_token *tok)
{
	t_token	*next;

	while (tok)
	{
		next = tok->next;
		free(tok->word);
		free(tok);
		tok = next;
	}
}
