#include "minishell.h"

// cd コマンドの実装
int	ft_cd(char **args, t_env **env)
{
	char	*path;
	char	*old_pwd;
	char	*new_pwd;

	if (!env || !*env)
		return (1);
	// 引数がない場合はHOMEディレクトリに移動
	if (!args || !args[1])
	{
		path = get_env_value(*env, "HOME");
		if (!path)
		{
			print_error("cd", NULL, "HOME not set");
			return (1);
		}
	}
	else
		path = args[1];
	// 現在のディレクトリを保存
	old_pwd = getcwd(NULL, 0);
	if (!old_pwd)
		old_pwd = ft_strdup("");
	// ディレクトリを変更
	if (chdir(path) != 0)
	{
		print_error("cd", path, strerror(errno));
		free(old_pwd);
		return (1);
	}
	// 新しいディレクトリを取得
	new_pwd = getcwd(NULL, 0);
	if (!new_pwd)
	{
		print_error("cd", path, "getcwd failed");
		free(old_pwd);
		return (1);
	}
	// PWDとOLDPWDを更新
	update_env_value(*env, "OLDPWD", old_pwd);
	update_env_value(*env, "PWD", new_pwd);
	free(old_pwd);
	free(new_pwd);
	return (0);
}








