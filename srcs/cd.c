/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 04:58:44 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 23:05:25 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// 新しい関数: 環境変数の PWD と OLDPWD を更新する (引数3個)
static int	update_pwd_env(t_env **env, char *old_pwd, char *path)
{
	char	*new_pwd;

	new_pwd = getcwd(NULL, 0);
	if (!new_pwd)
	{
		print_error("cd", path, "getcwd failed");
		free(old_pwd);
		return (1);
	}
	// OLDPWDを更新または追加
	if (get_env_value(*env, "OLDPWD") == NULL)
		add_env_node(env, "OLDPWD", old_pwd);
	else
		update_env_value(*env, "OLDPWD", old_pwd);
	// PWDを更新または追加
	if (get_env_value(*env, "PWD") == NULL)
		add_env_node(env, "PWD", new_pwd);
	else
		update_env_value(*env, "PWD", new_pwd);
	free(old_pwd);
	free(new_pwd);
	return (0);
}

// 新しい関数 1: 移動先のパスを決定する (引数2個)
// 成功: パス文字列 (args[1] または HOMEの値) を返す
// 失敗: NULL を返す (エラーメッセージはここで出力済み)
static char	*get_cd_path(char **args, t_env *env_list)
{
	char	*path;

	if (!args || !args[1])
	{
		path = get_env_value(env_list, "HOME");
		if (!path)
		{
			print_error("cd", NULL, "HOME not set");
			return (NULL);
		}
	}
	else
		path = args[1];
	return (path);
}

// 新しい関数 2: ディレクトリ変更と環境変数更新を実行する (引数3個)
static int	execute_cd(char *path, t_env **env)
{
	char	*old_pwd;
	int		status;

	old_pwd = getcwd(NULL, 0);
	if (!old_pwd)
		old_pwd = ft_strdup("");
	if (chdir(path) != 0)
	{
		print_error("cd", path, strerror(errno));
		free(old_pwd);
		return (1);
	}
	status = update_pwd_env(env, old_pwd, path);
	return (status);
}

// メイン関数 (制御とエラーチェック) (引数2個)
int	ft_cd(char **args, t_env **env)
{
	char	*path;

	if (!env || !*env)
		return (1);
	path = get_cd_path(args, *env);
	if (!path)
		return (1);
	return (execute_cd(path, env));
}
