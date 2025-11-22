/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 05:25:00 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 05:25:00 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_env_pair	parse_env_string(char *env_str)
{
	t_env_pair	pair;
	char		*equal_pos;

	equal_pos = ft_strchr(env_str, '=');
	if (equal_pos)
	{
		*equal_pos = '\0';
		pair.key = env_str;
		pair.value = equal_pos + 1;
	}
	else
	{
		pair.key = env_str;
		pair.value = NULL;
	}
	return (pair);
}

static int	add_parsed_node(t_env **env_list, t_env_pair pair)
{
	t_env	*new_node;

	new_node = create_env_node(pair.key, pair.value);
	if (!new_node)
	{
		free_env_list(*env_list);
		return (0);
	}
	add_env_node(env_list, pair.key, pair.value);
	return (1);
}

t_env	*init_env(char **envp)
{
	t_env		*env;
	t_env_pair	pair;
	int			i;

	env = NULL;
	if (!envp)
		return (NULL);
	i = 0;
	while (envp[i])
	{
		pair = parse_env_string(envp[i]);
		if (!add_parsed_node(&env, pair))
			return (NULL);
		i++;
	}
	return (env);
}

t_env	*create_env_node(char *key, char *value)
{
	t_env	*node;

	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	node->key = ft_strdup(key);
	if (value)
		node->value = ft_strdup(value);
	else
		node->value = NULL;
	node->next = NULL;
	if (!node->key || (value && !node->value))
	{
		free(node->key);
		free(node->value);
		free(node);
		return (NULL);
	}
	return (node);
}
