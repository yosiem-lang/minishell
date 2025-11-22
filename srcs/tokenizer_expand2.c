/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_expand2.c                                :+:      :+:    :+:   */
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

int	handle_dollar_expansion(char **result,
		char **p, t_env *env, int g_signal)
{
	char	*var_start;
	char	*var_end;

	var_start = *p;
	var_end = var_start + 1;
	while (*var_end && (ft_isalnum(*var_end) || *var_end == '_'))
		var_end++;
	if (var_end - var_start > 1)
	{
		if (expand_variable_name(result, p, env, var_end))
			return (1);
	}
	else if (*var_end == '?')
	{
		if (expand_special_variable(result, p, &g_signal, var_end))
			return (1);
	}
	return (0);
}

static void	process_quote_content(char **result,
		char **p, t_env *env, char quote_char)
{
	int	ret;

	while (**p && **p != quote_char)
	{
		if (quote_char == DOUBLE_QUOTE_CHAR && **p == '$')
		{
			ret = handle_dollar_expansion(result, p, env, g_signal);
			if (ret == 1)
				continue ;
		}
		append_char(result, **p);
		(*p)++;
	}
}

static void	check_unclosed_quote(char quote_char)
{
	if (quote_char == SINGLE_QUOTE_CHAR)
		print_error("minishell", NULL, "unclosed single quote");
	else
		print_error("minishell", NULL, "unclosed double quote");
}

void	expand_in_quotes(char **result,
		char **p, t_env *env, char quote_char)
{
	(*p)++;
	process_quote_content(result, p, env, quote_char);
	if (**p == '\0')
	{
		check_unclosed_quote(quote_char);
		return ;
	}
	(*p)++;
}
