#include "minishell.h"

// echo コマンドの実装
int	ft_echo(char **args, t_env **env)
{
	int	i;
	int	newline;

	(void)env;
	if (!args || !args[1])
	{
		ft_putchar_fd('\n', STDOUT_FILENO);
		return (0);
	}
	i = 1;
	newline = 1;
	// -n オプションのチェック
	if (ft_strncmp(args[1], "-n", 3) == 0)
	{
		newline = 0;
		i = 2;
	}
    // 引数を出力（環境変数展開）
    while (args[i])
	{
        if (env && *env && args[i][0] == '$' && args[i][1] != '\0')
        {
            char *val = get_env_value(*env, args[i] + 1);
            if (val)
                ft_putstr_fd(val, STDOUT_FILENO);
        }
        else
        {
            if (env && *env)
                expand_variables(&args[i], *env);
            ft_putstr_fd(args[i], STDOUT_FILENO);
        }
		if (args[i + 1])
			ft_putchar_fd(' ', STDOUT_FILENO);
		i++;
	}
	if (newline)
		ft_putchar_fd('\n', STDOUT_FILENO);
	return (0);
}
