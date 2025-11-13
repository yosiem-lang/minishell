#include "minishell.h"

// env コマンドの実装
int	ft_env(char **args, t_env **env)
{
	t_env	*current;

	(void)args;
	if (!env || !*env)
		return (1);
	current = *env;
	while (current)
	{
		if (current->value)
		{
			ft_putstr_fd(current->key, STDOUT_FILENO);
			ft_putchar_fd('=', STDOUT_FILENO);
			ft_putendl_fd(current->value, STDOUT_FILENO);
		}
		current = current->next;
	}
	return (0);
}









