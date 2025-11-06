#include "minishell.h"

// export コマンドの実装
int	ft_export(char **args, t_env **env)
{
	int		i;
	char	*key;
	char	*value;
	char	*equal_pos;

	if (!env || !*env)
		return (1);
	// 引数がない場合は環境変数を表示
	if (!args || !args[1])
		return (print_export_list(*env));
    i = 1;
    while (args[i])
    {
        // = の位置を探し、key/value を分離
        equal_pos = ft_strchr(args[i], '=');
        if (equal_pos)
        {
            *equal_pos = '\0';
            key = args[i];
            value = equal_pos + 1;
        }
        else
        {
            key = args[i];
            value = NULL;
        }
        // 有効な識別子かチェック（key のみ）
        if (!is_valid_identifier(key))
        {
            print_error("export", key, "not a valid identifier");
            i++;
            continue;
        }
		// 環境変数を追加または更新
		if (get_env_value(*env, key))
			update_env_value(*env, key, value);
		else
			add_env_node(env, key, value);
		i++;
	}
	return (0);
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
