#include "minishell.h"

// メイン関数
int	main(int argc, char **argv, char **envp)
{
	t_env	*env;

	(void)argc;
	(void)argv;
	// シグナルハンドラーを設定
	setup_signal_handlers();
	// 環境変数を初期化
	env = init_env(envp);
	if (!env)
	{
		ft_putendl_fd("minishell: failed to initialize environment", STDERR_FILENO);
		return (1);
	}
	// メインループを開始
	minishell_loop(&env);
	// クリーンアップ
	free_env_list(env);
	return (0);
}

// メインループ
int	minishell_loop(t_env **env)
{
	char	*input;
	char	*prompt;

	prompt = "minishell$ ";
	while (1)
	{
		// プロンプトを表示して入力を受け取る
		input = readline(prompt);
		if (!input)
		{
			ft_putendl_fd("exit", STDOUT_FILENO);
			break;
		}
		// 空でない場合は履歴に追加
		if (ft_strlen(input) > 0)
			add_history(input);
		// コマンドを解析・実行
		parse_and_execute(input, env);
		free(input);
	}
	return (0);
}

// コマンド引数とリダイレクションを分離
static char	**extract_args(char **tokens)
{
	char	**args;
	int		count;
	int		i;
	int		j;

	count = 0;
	i = 0;
	while (tokens[i])
	{
		if (ft_strncmp(tokens[i], "<", 2) == 0 || ft_strncmp(tokens[i], ">", 2) == 0 ||
			ft_strncmp(tokens[i], ">>", 3) == 0 || ft_strncmp(tokens[i], "<<", 3) == 0)
		{
			i += 2;
			continue;
		}
		if (ft_strncmp(tokens[i], "|", 2) == 0)
			break;
		count++;
		i++;
	}
	if (count == 0)
		return (NULL);
	args = malloc(sizeof(char *) * (count + 1));
	if (!args)
		return (NULL);
	i = 0;
	j = 0;
	while (tokens[i] && j < count)
	{
		if (ft_strncmp(tokens[i], "<", 2) == 0 || ft_strncmp(tokens[i], ">", 2) == 0 ||
			ft_strncmp(tokens[i], ">>", 3) == 0 || ft_strncmp(tokens[i], "<<", 3) == 0)
		{
			i += 2;
			continue;
		}
		if (ft_strncmp(tokens[i], "|", 2) == 0)
			break;
		args[j++] = ft_strdup(tokens[i++]);
		if (!args[j - 1])
		{
			while (j > 0)
				free(args[--j]);
			free(args);
			return (NULL);
		}
	}
	args[j] = NULL;
	return (args);
}

// 入力の解析と実行
void	parse_and_execute(char *input, t_env **env)
{
	char	**tokens;
	char	**args;
	int		in_fd;
	int		out_fd;
	int		saved_stdin;
	int		saved_stdout;

	// 入力が空の場合は何もしない
	if (!input || ft_strlen(input) == 0)
		return;
	// トークンに分割
	tokens = ft_split(input, ' ');
	if (!tokens)
		return;
	// コマンド引数を抽出
	args = extract_args(tokens);
	if (!args || !args[0])
	{
		free_array(tokens);
		if (args)
			free(args);
		return;
	}
	// リダイレクションを設定
	if (setup_redirections(tokens, &in_fd, &out_fd) != 0)
	{
		free_array(tokens);
		free(args);
		return;
	}
	// 標準入出力を保存
	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	// リダイレクションを適用
	if (in_fd >= 0)
	{
		dup2(in_fd, STDIN_FILENO);
		close(in_fd);
	}
	if (out_fd >= 0)
	{
		dup2(out_fd, STDOUT_FILENO);
		close(out_fd);
	}
	// コマンドを実行
	if (is_builtin(args[0]))
		execute_builtin(args, env);
	else
		execute_external(args, env);
	// ファイルディスクリプタを復元
	restore_fds(saved_stdin, saved_stdout);
	// メモリを解放
	free_array(tokens);
	free(args);
}
