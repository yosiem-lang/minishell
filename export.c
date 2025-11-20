#include "minishell.h"

int	ft_export(char **args, t_env **env)
{
	int	i;
	int	status;

	if (!env || !*env)
		return (1);
	if (!args || !args[1])
		return (print_export_list(*env));
	i = 1;
	status = 0;
	while (args[i])
	{
		if (handle_export_argument(args[i], env) != 0)
			status = 1;
		i++;
	}
	return (status);
}

// 引数: arg (入力文字列), env (環境リストへのポインタ) -> 引数2個
int	handle_export_argument(char *arg, t_env **env)
{
	char	*key;
	char	*value;
	char	*equal_pos;
	int		status;

	equal_pos = ft_strchr(arg, '=');
	if (equal_pos)
	{
		*equal_pos = '\0';
		key = arg;
		value = equal_pos + 1;
	}
	else
	{
		key = arg;
		value = NULL;
	}
	status = process_env_update(env, key, value, equal_pos);
	return (status);
}

// 引数: env (環境リスト), key (キー), value (値), equal_pos ('=' の位置ポインタ) -> 引数4個
int	process_env_update(t_env **env, char *key, char *value, char *equal_pos)
{
	if (!is_valid_identifier(key))
	{
		if (equal_pos)
			*equal_pos = '=';
		print_error("export", key, "not a valid identifier");
		return (1);
	}
	if (get_env_value(*env, key))
		update_env_value(*env, key, value);
	else
		add_env_node(env, key, value);
	if (equal_pos)
		*equal_pos = '=';
	return (0);
}

// パース結果を格納するためのシンプルな構造体（引数削減の要件に反しないように注意）
// または、in/outパラメータ（ポインタ）を使用して引数を4つ以下に保つ。
void	extract_key_value(char *arg, char **key_out, char **value_out)
{
	char	*equal_pos;

	equal_pos = ft_strchr(arg, '=');
	if (equal_pos)
	{
		*equal_pos = '\0';
		*key_out = arg;
		*value_out = equal_pos + 1;
	}
	else
	{
		*key_out = arg;
		*value_out = NULL;
	}
}

// export リストを表示
int	print_export_list(t_env *env)
{
	t_env	*current;

	if (!env)
		return (1);
	current = env;
	while (current)
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		ft_putstr_fd(current->key, STDOUT_FILENO);
		if (current->value)
		{
			ft_putstr_fd("=\"", STDOUT_FILENO);
			ft_putstr_fd(current->value, STDOUT_FILENO);
			ft_putstr_fd("\"", STDOUT_FILENO);
		}
		ft_putchar_fd('\n', STDOUT_FILENO);
		current = current->next;
	}
	return (0);
}
