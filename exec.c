#include "minishell.h"

static char	*join_path(const char *dir, const char *cmd)
{
	char	*tmp;
	char	*full;

	tmp = ft_strjoin(dir, "/");
	if (!tmp)
		return (NULL);
	full = ft_strjoin(tmp, cmd);
	free(tmp);
	return (full);
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

// 新しい関数 1: 子プロセスでの処理 (引数3個)
// 成功した場合は execve が返らないため、失敗時のエラー処理のみ
static void	execute_child_process(char *exec_path, char **argv, t_env **env)
{
	char	**envp;

	envp = env_to_array(*env);
	if (!envp)
		_exit(1);
	execve(exec_path, argv, envp);
	perror("minishell");
	_exit(126);
}

// 新しい関数 2: 親プロセスでの待機とステータス解析 (引数1個)
static int	wait_for_child(pid_t pid)
{
	int	status;

	if (waitpid(pid, &status, 0) < 0)
		return (1);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

// メイン関数 (引数2個: argv, env)
int	execute_external(char **argv, t_env **env)
{
	pid_t	pid;
	char	*exec_path;
	int		exit_status;

	if (!argv || !argv[0])
		return (1);
	exec_path = find_executable(argv[0], *env);
	if (!exec_path)
	{
		print_error(argv[0], NULL, "command not found");
		return (127);
	}
	pid = fork();
	if (pid < 0)
	{
		free(exec_path);
		print_error(argv[0], NULL, strerror(errno));
		return (1);
	}
	if (pid == 0)
		execute_child_process(exec_path, argv, env);
	free(exec_path);
	exit_status = wait_for_child(pid);
	return (exit_status);
}
