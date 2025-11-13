#include "minishell.h"

// ビルトインコマンドのテーブル
static t_builtin	g_builtins[] = {
	{"echo", ft_echo},
	{"cd", ft_cd},
	{"pwd", ft_pwd},
	{"export", ft_export},
	{"unset", ft_unset},
	{"env", ft_env},
	{"exit", ft_exit},
	{"cat", ft_cat},
	{NULL, NULL}
};

// ビルトインコマンドかどうかをチェック
int	is_builtin(char *cmd)
{
	int	i;

	if (!cmd)
		return (0);
	i = 0;
	while (g_builtins[i].name)
	{
		if (ft_strncmp(cmd, g_builtins[i].name, ft_strlen(g_builtins[i].name) + 1) == 0)
			return (1);
		i++;
	}
	return (0);
}

// ビルトインコマンドを実行（ディスパッチ）
int	execute_builtin(char **args, t_env **env)
{
	int	i;

	if (!args || !args[0])
		return (1);
	i = 0;
	while (g_builtins[i].name)
	{
		if (ft_strncmp(args[0], g_builtins[i].name, ft_strlen(g_builtins[i].name) + 1) == 0)
			return (g_builtins[i].func(args, env));
		i++;
	}
	return (1);
}
