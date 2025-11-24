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

// 安全なメモリ確保関数（エラー時にNULLを返す）
static void	*safe_malloc(size_t size)
{
	void	*ptr;

	ptr = malloc(size);
	if (!ptr)
	{
		print_error(NULL, NULL, "malloc failed");
		return (NULL);
	}
	return (ptr);
}

int	replace_variable(char **str, char *start, char *end, char *value)
{
	char	*new_str;
	int		len;
	int		value_len;
	size_t	new_len;

	if (!str || !*str || !start || !end || !value)
		return (-1);
	len = ft_strlen(*str);
	value_len = ft_strlen(value);
	new_len = len - (end - start) + value_len + 1;
	if (new_len == 0)
		return (-1);
	new_str = safe_malloc(new_len);
	if (!new_str)
		return (-1);
	ft_strlcpy(new_str, *str, start - *str + 1);
	ft_strlcat(new_str, value, new_len);
	ft_strlcat(new_str, end, new_len);
	free(*str);
	*str = new_str;
	return (0);
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
