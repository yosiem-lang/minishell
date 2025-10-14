#include "minishell.h"

void	fatal_error(const char *msg)
{
	dprintf(STDERR_FILENO, "Fatal Error: %s\n", msg);
	exit(1);
}

/*-----------------------------------------------------------
** PATH環境変数を探索して、コマンドの実行可能パスを探す
-----------------------------------------------------------*/
char	*search_path(const char *filename)
{
	char	path[PATH_MAX];
	char	*value;
	char	*end;

	if (strchr(filename, '/'))
		return (strdup(filename)); // すでにパス付きならそのまま使う

	value = getenv("PATH");
	if (!value)
		return (NULL);
	while (*value)
	{
		bzero(path, PATH_MAX);
		end = strchr(value, ':');
		if (end)
			strncpy(path, value, end - value);
		else
			strlcpy(path, value, PATH_MAX);
		strlcat(path, "/", PATH_MAX);
		strlcat(path, filename, PATH_MAX);
		if (access(path, X_OK) == 0)
			return (strdup(path));
		if (end == NULL)
			break;
		value = end + 1;
	}
	return (NULL);
}

/*-----------------------------------------------------------
** スペースで区切って argv[] を作る簡易トークナイザ
-----------------------------------------------------------*/
char	**tokenize(char *line)
{
	char	**argv;
	char	*token;
	int		count = 0;

	argv = calloc(64, sizeof(char *)); // 最大64個の引数まで対応（簡易版）
	if (!argv)
		fatal_error("calloc");

	token = strtok(line, " \t\n");
	while (token && count < 63)
	{
		argv[count++] = strdup(token);
		token = strtok(NULL, " \t\n");
	}
	argv[count] = NULL;
	return (argv);
}

/*-----------------------------------------------------------
** コマンド実行部分
-----------------------------------------------------------*/
int	interpret(char *line)
{
	extern char	**environ;
	char		**argv;
	char		*path;
	pid_t		pid;
	int			wstatus;

	argv = tokenize(line);
	if (!argv[0])
		return (0);

	path = search_path(argv[0]);
	if (!path)
	{
		dprintf(STDERR_FILENO, "Command not found: %s\n", argv[0]);
		return (127);
	}

	pid = fork();
	if (pid < 0)
		fatal_error("fork");
	else if (pid == 0)
	{
		execve(path, argv, environ);
		fatal_error("execve");
	}
	else
	{
		waitpid(pid, &wstatus, 0);
	}

	free(path);
	for (int i = 0; argv[i]; i++)
		free(argv[i]);
	free(argv);
	return (WEXITSTATUS(wstatus));
}

/*-----------------------------------------------------------
** メインループ
-----------------------------------------------------------*/
int	main(void)
{
	char	*line;

	rl_outstream = stderr;
	while (1)
	{
		line = readline("minishell$ ");
		if (line == NULL)
			break;
		if (*line)
		{
			add_history(line);
			interpret(line);
		}
		free(line);
	}
	printf("exit\n");
	return (0);
}
