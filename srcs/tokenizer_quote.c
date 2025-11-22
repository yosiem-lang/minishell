/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_quote.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 05:05:00 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 05:05:00 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define SINGLE_QUOTE_CHAR '\''
#define DOUBLE_QUOTE_CHAR '\"'

int	remove_quotes_from_word(char *word, char **new_word_ptr)
{
	char	*p;
	int		ret;

	p = word;
	while (*p)
	{
		ret = handle_quoted_segment(&p, new_word_ptr, SINGLE_QUOTE_CHAR);
		if (ret == 1)
			continue ;
		if (ret == -1)
			return (-1);
		ret = handle_quoted_segment(&p, new_word_ptr, DOUBLE_QUOTE_CHAR);
		if (ret == 1)
			continue ;
		if (ret == -1)
			return (-1);
		append_char(new_word_ptr, *p++);
	}
	return (0);
}

void	quote_removal(t_token *tok)
{
	char	*new_word;
	int		ret;

	while (tok && tok->kind != TK_EOF)
	{
		new_word = NULL;
		ret = remove_quotes_from_word(tok->word, &new_word);
		if (ret == -1)
		{
			if (new_word)
				free(new_word);
			new_word = ft_strdup("");
		}
		free(tok->word);
		tok->word = new_word;
		tok = tok->next;
	}
}
