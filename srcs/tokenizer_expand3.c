/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_expand3.c                                :+:      :+:    :+:   */
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

int	handle_unquoted_var(char **new_word,
		char **p, t_env *env, int exit_status)
{
	char	*var_start;
	char	*var_end;
	int		ret;

	var_start = *p;
	var_end = *p + 1;
	while (*var_end && (ft_isalnum(*var_end) || *var_end == '_'))
		var_end++;
	if (var_end - var_start > 1)
	{
		if (expand_regular_unquoted(new_word, var_start, var_end, env))
		{
			*p = var_end;
			return (1);
		}
	}
	ret = handle_special_unquoted(new_word, p, var_end, exit_status);
	return (ret);
}

void	process_current_char(char **new_word, char **p)
{
	append_char(new_word, **p);
	(*p)++;
}

void	process_single_token(char **new_word,
		char **p, t_env *env, int g_signal)
{
	int	ret;

	while (**p)
	{
		if (**p == SINGLE_QUOTE_CHAR)
			handle_single_quote(new_word, p);
		else if (**p == DOUBLE_QUOTE_CHAR)
			expand_in_quotes(new_word, p, env, DOUBLE_QUOTE_CHAR);
		else if (**p == '$')
		{
			ret = handle_unquoted_var(new_word, p, env, g_signal);
			if (ret == 1)
				continue ;
			process_current_char(new_word, p);
		}
		else
			process_current_char(new_word, p);
	}
}

void	expand_and_remove_quotes(t_token *tok, t_env *env)
{
	char	*new_word;
	char	*p;
	char	**p_ptr;
	int		g_signal_val;

	g_signal_val = g_signal;
	while (tok && tok->kind != TK_EOF)
	{
		new_word = NULL;
		p = tok->word;
		p_ptr = &p;
		process_single_token(&new_word, p_ptr, env, g_signal_val);
		free(tok->word);
		tok->word = new_word;
		tok = tok->next;
	}
}

int	handle_quoted_segment(char **p_ptr,
		char **new_word_ptr, char quote_char)
{
	char	*p;

	p = *p_ptr;
	if (*p != quote_char)
		return (0);
	p++;
	while (*p && *p != quote_char)
		append_char(new_word_ptr, *p++);
	if (*p == '\0')
	{
		if (quote_char == SINGLE_QUOTE_CHAR)
			print_error("minishell", NULL, "unclosed single quote");
		else
			print_error("minishell", NULL, "unclosed double quote");
		return (-1);
	}
	p++;
	*p_ptr = p;
	return (1);
}
