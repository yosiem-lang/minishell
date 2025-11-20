#include "minishell.h"

// pwd コマンドの実装
int	ft_pwd(char **args, t_env **env)
{
	char	*cwd;

	(void)args;
	(void)env;
	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		print_error("pwd", NULL, strerror(errno));
		return (1);
	}
	ft_putendl_fd(cwd, STDOUT_FILENO);
	free(cwd);
	return (0);
}
