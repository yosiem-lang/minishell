/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_expand4.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 05:43:36 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 05:48:54 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define SINGLE_QUOTE_CHAR '\''

void	handle_single_quote(char **new_word, char **p)
{
	(*p)++;
	while (**p && **p != SINGLE_QUOTE_CHAR)
		append_char(new_word, *(*p)++);
	if (**p == '\0')
	{
		print_error("minishell", NULL, "unclosed single quote");
		return ;
	}
	(*p)++;
}

int	expand_regular_unquoted(char **new_word,
		char *var_start, char *var_end, t_env *env)
{
	char	*var_name;
	char	*var_value;
	int		len;

	len = var_end - var_start - 1;
	if (len <= 0)
		return (0);
	var_name = ft_substr(var_start + 1, 0, len);
	if (!var_name)
		fatal_error("ft_substr");
	var_value = get_env_value(env, var_name);
	if (var_value)
		*new_word = strjoin_free(*new_word, var_value);
	free(var_name);
	return (1);
}

int	handle_special_unquoted(char **new_word,
		char **p_ptr, char *var_end, int exit_status)
{
	char	*var_value;

	if (*var_end == '?')
	{
		var_value = ft_itoa(exit_status);
		if (var_value)
		{
			*new_word = strjoin_free(*new_word, var_value);
			free(var_value);
		}
		*p_ptr = var_end + 1;
		return (1);
	}
	return (0);
}
