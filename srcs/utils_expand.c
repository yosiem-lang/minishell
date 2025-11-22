/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_expand.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 05:43:56 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 05:48:54 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	expand_variables(char **str, t_env *env)
{
	char	*expanded;

	if (!str || !*str || !env)
		return ;
	expanded = get_expanded_string(*str, env);
	if (expanded)
	{
		free(*str);
		*str = expanded;
	}
}

static int	expand_regular_variable(char **result_ptr,
		t_env *env, char *var_start, char *var_end)
{
	char	*var_name;
	char	*var_value;
	int		len;

	len = var_end - var_start - 1;
	if (len <= 0)
		return (0);
	var_name = ft_substr(var_start + 1, 0, len);
	if (!var_name)
		return (-1);
	var_value = get_env_value(env, var_name);
	if (var_value)
		replace_variable(result_ptr, var_start, var_end, var_value);
	else
		replace_variable(result_ptr, var_start, var_end, "");
	free(var_name);
	return (1);
}

static int	expand_special_var_utils(char **result_ptr,
		char *var_start, char *var_end, int exit_status)
{
	char	*var_value;

	if (var_end[0] != '?')
		return (0);
	var_value = ft_itoa(exit_status);
	if (!var_value)
		return (-1);
	replace_variable(result_ptr, var_start, var_end + 1, var_value);
	free(var_value);
	return (1);
}

static int	process_next_variable(char **result_ptr,
		t_env *env, char *var_start, int g_signal)
{
	char	*var_end;
	int		ret;

	var_end = var_start + 1;
	while (var_end[0] && (ft_isalnum(var_end[0]) || var_end[0] == '_'))
		var_end++;
	ret = expand_regular_variable(result_ptr, env, var_start, var_end);
	if (ret != 0)
		return (ret);
	ret = expand_special_var_utils(result_ptr, var_start, var_end, g_signal);
	if (ret != 0)
		return (ret);
	return (0);
}

char	*get_expanded_string(char *str, t_env *env)
{
	char	*result;
	char	*var_start;
	int		ret;

	if (!str || !env)
		return (NULL);
	result = ft_strdup(str);
	if (!result)
		return (NULL);
	var_start = ft_strchr(result, '$');
	while (var_start)
	{
		ret = process_next_variable(&result, env, var_start, g_signal);
		if (ret == -1)
			return (free(result), NULL);
		if (ret == 1)
		{
			var_start = ft_strchr(result, '$');
			continue ;
		}
		var_start = ft_strchr(var_start + 1, '$');
	}
	return (result);
}
