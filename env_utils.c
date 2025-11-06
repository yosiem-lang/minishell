#include "minishell.h"

// 環境変数リストを初期化
t_env	*init_env(char **envp)
{
	t_env	*env;
	t_env	*new_node;
	char	*equal_pos;
	char	*key;
	char	*value;
	int		i;

	env = NULL;
	if (!envp)
		return (NULL);
	i = 0;
	while (envp[i])
	{
		equal_pos = ft_strchr(envp[i], '=');
		if (equal_pos)
		{
			*equal_pos = '\0';
			key = envp[i];
			value = equal_pos + 1;
		}
		else
		{
			key = envp[i];
			value = NULL;
		}
		new_node = create_env_node(key, value);
		if (!new_node)
		{
			free_env_list(env);
			return (NULL);
		}
		add_env_node(&env, key, value);
		i++;
	}
	return (env);
}

// 環境変数ノードを作成
t_env	*create_env_node(char *key, char *value)
{
	t_env	*node;

	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	node->key = ft_strdup(key);
	node->value = value ? ft_strdup(value) : NULL;
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

// 環境変数ノードを追加
void	add_env_node(t_env **env, char *key, char *value)
{
	t_env	*new_node;
	t_env	*current;

	new_node = create_env_node(key, value);
	if (!new_node)
		return;
	if (!*env)
	{
		*env = new_node;
		return;
	}
	current = *env;
	while (current->next)
		current = current->next;
	current->next = new_node;
}

// 環境変数の値を取得
char	*get_env_value(t_env *env, char *key)
{
	t_env	*current;

	if (!env || !key)
		return (NULL);
	current = env;
	while (current)
	{
		if (ft_strncmp(current->key, key, ft_strlen(key) + 1) == 0)
			return (current->value);
		current = current->next;
	}
	return (NULL);
}

// 環境変数の値を更新
void	update_env_value(t_env *env, char *key, char *value)
{
	t_env	*current;

	if (!env || !key)
		return;
	current = env;
	while (current)
	{
		if (ft_strncmp(current->key, key, ft_strlen(key) + 1) == 0)
		{
			free(current->value);
			current->value = value ? ft_strdup(value) : NULL;
			return;
		}
		current = current->next;
	}
}

// 環境変数ノードを削除
void	remove_env_node(t_env **env, char *key)
{
	t_env	*current;
	t_env	*prev;

	if (!env || !*env || !key)
		return;
	current = *env;
	prev = NULL;
	while (current)
	{
		if (ft_strncmp(current->key, key, ft_strlen(key) + 1) == 0)
		{
			if (prev)
				prev->next = current->next;
			else
				*env = current->next;
			free(current->key);
			free(current->value);
			free(current);
			return;
		}
		prev = current;
		current = current->next;
	}
}

// 環境変数リストを解放
void	free_env_list(t_env *env)
{
	t_env	*current;
	t_env	*next;

	current = env;
	while (current)
	{
		next = current->next;
		free(current->key);
		free(current->value);
		free(current);
		current = next;
	}
}

// 環境変数リストを配列に変換
char	**env_to_array(t_env *env)
{
	char	**array;
	t_env	*current;
	int		count;
	int		i;

	count = 0;
	current = env;
	while (current)
	{
		count++;
		current = current->next;
	}
	array = malloc(sizeof(char *) * (count + 1));
	if (!array)
		return (NULL);
	i = 0;
	current = env;
	while (current)
	{
		if (current->value)
		{
			array[i] = malloc(ft_strlen(current->key) + ft_strlen(current->value) + 2);
			if (!array[i])
			{
				free_array(array);
				return (NULL);
			}
		ft_strlcpy(array[i], current->key, ft_strlen(current->key) + 1);
		ft_strlcat(array[i], "=", ft_strlen(array[i]) + 2);
		ft_strlcat(array[i], current->value, ft_strlen(array[i]) + ft_strlen(current->value) + 1);
		}
		else
			array[i] = ft_strdup(current->key);
		i++;
		current = current->next;
	}
	array[i] = NULL;
	return (array);
}

