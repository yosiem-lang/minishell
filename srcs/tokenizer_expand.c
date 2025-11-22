/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_expand.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 05:05:00 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 05:05:00 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	append_char(char **s, char c)
{
	size_t	size;
	char	*new;

	size = 2;
	if (*s)
		size += strlen(*s);
	new = malloc(size);
	if (new == NULL)
		fatal_error("malloc");
	if (*s)
		strlcpy(new, *s, size);
	new[size - 2] = c;
	new[size - 1] = '\0';
	if (*s)
		free(*s);
	*s = new;
}

char	*strjoin_free(char *s1, char *s2)
{
	char	*result;

	if (!s1 && !s2)
		return (NULL);
	if (!s1)
		return (ft_strdup(s2));
	if (!s2)
		return (s1);
	result = ft_strjoin(s1, s2);
	free(s1);
	return (result);
}

static int	extract_and_lookup_var(char **p,
		t_env *env, char *var_end, char **expanded_part_ptr)
{
	char	*var_start;
	int		len;
	char	*var_name;
	char	*var_value;

	var_start = *p;
	len = var_end - var_start - 1;
	if (len <= 0)
		return (0);
	var_name = ft_substr(var_start + 1, 0, len);
	if (!var_name)
		fatal_error("ft_substr");
	var_value = get_env_value(env, var_name);
	if (var_value)
		*expanded_part_ptr = ft_strdup(var_value);
	else
		*expanded_part_ptr = ft_strdup("");
	if (!*expanded_part_ptr)
	{
		free(var_name);
		fatal_error("ft_strdup");
	}
	free(var_name);
	return (1);
}

int	expand_variable_name(char **result,
		char **p, t_env *env, char *var_end)
{
	char	*expanded_part;
	int		ret;

	ret = extract_and_lookup_var(p, env, var_end, &expanded_part);
	if (ret == 0)
		return (0);
	*result = strjoin_free(*result, expanded_part);
	free(expanded_part);
	*p = var_end;
	return (1);
}

int	expand_special_variable(char **result,
		char **p, int *exit_status_ptr, char *var_end)
{
	char	*var_start;
	char	*var_value;

	var_start = *p;
	if (*(var_start + 1) != '?')
		return (0);
	var_value = ft_itoa(*exit_status_ptr);
	if (!var_value)
		fatal_error("ft_itoa");
	*result = strjoin_free(*result, var_value);
	free(var_value);
	*p = var_end + 1;
	return (1);
}
