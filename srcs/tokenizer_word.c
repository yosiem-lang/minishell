/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_word.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 05:05:00 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 23:05:25 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define SINGLE_QUOTE_CHAR '\''
#define DOUBLE_QUOTE_CHAR '\"'

static char	*scan_word_end(char *line)
{
	while (*line && !is_metacharacter(*line))
	{
		if (*line == SINGLE_QUOTE_CHAR)
		{
			line++;
			while (*line && *line != SINGLE_QUOTE_CHAR)
				line++;
			if (*line == '\0')
				fatal_error("Unclosed single quote");
			line++;
		}
		else if (*line == DOUBLE_QUOTE_CHAR)
		{
			line++;
			while (*line && *line != DOUBLE_QUOTE_CHAR)
				line++;
			if (*line == '\0')
				fatal_error("Unclosed double quote");
			line++;
		}
		else
			line++;
	}
	return (line);
}

t_token	*word(char **rest, char *line)
{
	const char	*start = line;
	char		*end;
	char		*word_str;

	end = scan_word_end(line);
	word_str = strndup(start, end - start);
	if (word_str == NULL)
		fatal_error("strndup");
	*rest = end;
	return (new_token(word_str, TK_WORD));
}
