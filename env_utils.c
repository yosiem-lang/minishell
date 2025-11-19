#include "minishell.h"

// 新しい関数 1: 環境変数文字列からキーと値をパースする (引数1個)
// 成功時はパース結果を含む構造体 (スタックに確保) を返す
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

// 新しい関数 2: パースされたキーと値でノードを作成し、リストに追加する (引数2個)
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

// メイン関数 (引数1個: envp)
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

// 環境変数ノードを作成
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

// 環境変数ノードを追加
void	add_env_node(t_env **env, char *key, char *value)
{
	t_env	*new_node;
	t_env	*current;

	new_node = create_env_node(key, value);
	if (!new_node)
		return ;
	if (!*env)
	{
		*env = new_node;
		return ;
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
		return ;
	current = env;
	while (current)
	{
		if (ft_strncmp(current->key, key, ft_strlen(key) + 1) == 0)
		{
			free(current->value);
			if (value)
				current->value = ft_strdup(value);
			else
				current->value = NULL;
			return ;
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
		return ;
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
			return ;
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

// 新しい関数 1: 環境リストのノード数をカウントする (引数1個)
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

// 新しい関数 2: 単一のノードから KEY=VALUE 文字列を作成する (引数1個)
// 失敗時には NULL を返す
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
			return (NULL);
		ft_strlcpy(str, node->key, total_len);
		ft_strlcat(str, "=", total_len);
		ft_strlcat(str, node->value, total_len);
	}
	return (str);
}

// 新しい関数 1: 環境リストの内容を配列にコピーする (引数3個)
// 成功時は1を返し、失敗時は0を返す (失敗時に配列を解放する)
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

// メイン関数 (引数1個: env)
char	**env_to_array(t_env *env)
{
	char	**array;
	int		count;

	count = count_env_nodes(env);
	array = malloc(sizeof(char *) * (count + 1));
	if (!array)
		return (NULL);
	if (!convert_env_list_to_array(env, array, count))
		return (NULL);
	return (array);
}
