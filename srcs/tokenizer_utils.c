/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 05:05:00 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/25 04:13:30 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	is_metacharacter(char c)
{
	return (c && ft_strchr("|&;()<> \t\n", c));
}

bool	startswith(const char *s, const char *keyword)
{
	return (ft_memcmp(s, keyword, ft_strlen(keyword)) == 0);
}

bool	is_operator(const char *s)
{
	static char *const	operators[] = {
		"||", "&", "&&", ";", ";;", "(", ")", "|", "\n", "<<", "<", ">>", ">"
	};
	size_t				i;

	i = 0;
	while (i < sizeof(operators) / sizeof(*operators))
	{
		if (startswith(s, operators[i]))
			return (true);
		i++;
	}
	return (false);
}

t_token	*operator(char **rest, char *line)
{
	static char *const	operators[] = {
		"||", "&", "&&", ";", ";;", "(", ")", "|", "\n", "<<", "<", ">>", ">"
	};
	size_t				i;
	char				*op;

	i = 0;
	while (i < sizeof(operators) / sizeof(*operators))
	{
		if (startswith(line, operators[i]))
		{
			op = ft_strdup(operators[i]);
			if (op == NULL)
				fatal_error("ft_strdup");
			*rest = line + ft_strlen(op);
			return (new_token(op, TK_OP));
		}
		i++;
	}
	assert_error("Unexpected operator");
	return (NULL);
}

bool	is_word(const char *s)
{
	return (*s && !is_metacharacter(*s));
}
