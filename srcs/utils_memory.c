/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_memory.c                                     :+:      :+:    :+:   */
/*                                                    ft +:+         +:+      */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 05:25:00 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 05:25:00 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	replace_variable(char **str, char *start, char *end, char *value)
{
	char	*new_str;
	int		len;
	int		value_len;

	if (!str || !*str || !start || !end || !value)
		return ;
	len = ft_strlen(*str);
	value_len = ft_strlen(value);
	new_str = malloc(len - (end - start) + value_len + 1);
	if (!new_str)
		return ;
	ft_strlcpy(new_str, *str, start - *str + 1);
	ft_strlcat(new_str, value, ft_strlen(new_str) + ft_strlen(value) + 1);
	ft_strlcat(new_str, end, ft_strlen(new_str) + ft_strlen(end) + 1);
	free(*str);
	*str = new_str;
}

void	free_array(char **array)
{
	int	i;

	if (!array)
		return ;
	i = 0;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}
