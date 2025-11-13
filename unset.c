#include "minishell.h"

// unset コマンドの実装
int	ft_unset(char **args, t_env **env)
{
	int	i;

	if (!env || !*env)
		return (1);
	if (!args || !args[1])
		return (0);
	i = 1;
	while (args[i])
	{
		// 有効な識別子かチェック
		if (!is_valid_identifier(args[i]))
		{
			print_error("unset", args[i], "not a valid identifier");
			i++;
			continue;
		}
		// 環境変数を削除
		remove_env_node(env, args[i]);
		i++;
	}
	return (0);
}













