/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_array.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 05:25:00 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/24 22:31:18 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	count_env_nodes(t_env *env)
{
	int		count;
	t_env	*current;

	count = 0;
	current = env;
	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}

static char	*create_env_string(t_env *node)
{
	char	*str;
	size_t	total_len;

	if (!node->value)
		str = ft_strdup(node->key);
	else
	{
		total_len = ft_strlen(node->key) + ft_strlen(node->value) + 2;
		str = malloc(total_len);
		if (!str)
		{
			print_error(NULL, NULL, "malloc failed");
			return (NULL);
		}
		ft_strlcpy(str, node->key, total_len);
		ft_strlcat(str, "=", total_len);
		ft_strlcat(str, node->value, total_len);
	}
	return (str);
}

static int	convert_env_list_to_array(t_env *env, char **array, int count)
{
	t_env	*current;
	int		i;
	int		j;

	i = 0;
	current = env;
	while (current)
	{
		array[i] = create_env_string(current);
		if (!array[i])
		{
			j = 0;
			while (j < i)
				free(array[j++]);
			free(array);
			return (0);
		}
		i++;
		current = current->next;
	}
	array[count] = NULL;
	return (1);
}

char	**env_to_array(t_env *env)
{
	char	**array;
	int		count;

	count = count_env_nodes(env);
	array = malloc(sizeof(char *) * (count + 1));
	if (!array)
	{
		print_error(NULL, NULL, "malloc failed");
		return (NULL);
	}
	if (!convert_env_list_to_array(env, array, count))
		return (NULL);
	return (array);
}
