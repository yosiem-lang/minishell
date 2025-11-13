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

// トークンリストを文字列配列に変換
static char	**tokens_to_array(t_token *tok)
{
	char	**array;
	int		count;
	int		i;
	t_token	*current;

	count = 0;
	current = tok;
	while (current && current->kind != TK_EOF)
	{
		count++;
		current = current->next;
	}
	if (count == 0)
		return (NULL);
	array = malloc(sizeof(char *) * (count + 1));
	if (!array)
		return (NULL);
	i = 0;
	current = tok;
	while (current && current->kind != TK_EOF)
	{
		array[i++] = ft_strdup(current->word);
		if (!array[i - 1])
		{
			while (i > 0)
				free(array[--i]);
			free(array);
			return (NULL);
		}
		current = current->next;
	}
	array[i] = NULL;
	return (array);
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

// パイプがあるかチェック
static int	has_pipe(char **tokens)
{
	int	i;

	i = 0;
	while (tokens[i])
	{
		if (ft_strncmp(tokens[i], "|", 2) == 0)
			return (1);
		i++;
	}
	return (0);
}

// パイプで分割されたコマンドの引数を取得
static char	**get_pipe_command(char **tokens, int *start_idx)
{
	char	**args;
	int		count;
	int		i;
	int		j;

	count = 0;
	i = *start_idx;
	while (tokens[i] && ft_strncmp(tokens[i], "|", 2) != 0)
	{
		if (ft_strncmp(tokens[i], "<", 2) == 0 || ft_strncmp(tokens[i], ">", 2) == 0 ||
			ft_strncmp(tokens[i], ">>", 3) == 0 || ft_strncmp(tokens[i], "<<", 3) == 0)
		{
			i += 2;
			continue;
		}
		count++;
		i++;
	}
	if (count == 0)
		return (NULL);
	args = malloc(sizeof(char *) * (count + 1));
	if (!args)
		return (NULL);
	i = *start_idx;
	j = 0;
	while (tokens[i] && ft_strncmp(tokens[i], "|", 2) != 0)
	{
		if (ft_strncmp(tokens[i], "<", 2) == 0 || ft_strncmp(tokens[i], ">", 2) == 0 ||
			ft_strncmp(tokens[i], ">>", 3) == 0 || ft_strncmp(tokens[i], "<<", 3) == 0)
		{
			i += 2;
			continue;
		}
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
	if (tokens[i] && ft_strncmp(tokens[i], "|", 2) == 0)
		*start_idx = i + 1;
	else
		*start_idx = i;
	return (args);
}

// パイプ内のコマンドのリダイレクションを設定
static int	setup_pipe_redirections(char **tokens, int start_idx, int end_idx, int *in_fd, int *out_fd)
{
	int	i;
	int	fd;

	if (!tokens || !in_fd || !out_fd)
		return (1);
	*in_fd = -1;
	*out_fd = -1;
	i = start_idx;
	while (i < end_idx && tokens[i])
	{
		if (ft_strncmp(tokens[i], ">>", 3) == 0 && ft_strlen(tokens[i]) == 2)
		{
			if (!tokens[i + 1] || i + 1 >= end_idx)
			{
				print_error("syntax error", NULL, "unexpected end of file");
				return (1);
			}
			if (*out_fd >= 0)
				close(*out_fd);
			fd = open_output_file(tokens[i + 1], 1);
			if (fd < 0)
				return (1);
			*out_fd = fd;
			i += 2;
		}
		else if (ft_strncmp(tokens[i], "<<", 3) == 0 && ft_strlen(tokens[i]) == 2)
		{
			if (!tokens[i + 1] || i + 1 >= end_idx)
			{
				print_error("syntax error", NULL, "unexpected end of file");
				return (1);
			}
			if (*in_fd >= 0)
				close(*in_fd);
			fd = handle_heredoc(tokens[i + 1]);
			if (fd < 0)
				return (1);
			*in_fd = fd;
			i += 2;
		}
		else if (ft_strncmp(tokens[i], "<", 2) == 0 && ft_strlen(tokens[i]) == 1)
		{
			if (!tokens[i + 1] || i + 1 >= end_idx)
			{
				print_error("syntax error", NULL, "unexpected end of file");
				return (1);
			}
			if (*in_fd >= 0)
				close(*in_fd);
			fd = open_input_file(tokens[i + 1]);
			if (fd < 0)
				return (1);
			*in_fd = fd;
			i += 2;
		}
		else if (ft_strncmp(tokens[i], ">", 2) == 0 && ft_strlen(tokens[i]) == 1)
		{
			if (!tokens[i + 1] || i + 1 >= end_idx)
			{
				print_error("syntax error", NULL, "unexpected end of file");
				return (1);
			}
			if (*out_fd >= 0)
				close(*out_fd);
			fd = open_output_file(tokens[i + 1], 0);
			if (fd < 0)
				return (1);
			*out_fd = fd;
			i += 2;
		}
		else
			i++;
	}
	return (0);
}

// パイプ処理を実行
static int	execute_pipe(char **tokens, t_env **env)
{
	char	**args;
	int		pipe_fd[2];
	int		prev_pipe_read;
	int		status;
	int		start_idx;
	int		cmd_start;
	int		end_idx;
	int		has_next;
	int		in_fd;
	int		out_fd;
	pid_t	pid;

	prev_pipe_read = -1;
	start_idx = 0;
	while (1)
	{
		// コマンドの開始位置を保存
		cmd_start = start_idx;
		// 次のパイプまたは終端を見つける
		end_idx = start_idx;
		while (tokens[end_idx] && ft_strncmp(tokens[end_idx], "|", 2) != 0)
			end_idx++;
		has_next = (tokens[end_idx] != NULL);
		// コマンド引数を取得
		args = get_pipe_command(tokens, &start_idx);
		if (!args || !args[0])
			break;
		// リダイレクションを設定（このコマンドの範囲内で）
		if (setup_pipe_redirections(tokens, cmd_start, end_idx, &in_fd, &out_fd) != 0)
		{
			free_array(args);
			return (1);
		}
		// 次のコマンドがある場合はパイプを作成
		if (has_next)
		{
			if (pipe(pipe_fd) < 0)
			{
				print_error("pipe", NULL, strerror(errno));
				free_array(args);
				if (in_fd >= 0)
					close(in_fd);
				if (out_fd >= 0)
					close(out_fd);
				return (1);
			}
		}
		pid = fork();
		if (pid < 0)
		{
			print_error("fork", NULL, strerror(errno));
			free_array(args);
			if (in_fd >= 0)
				close(in_fd);
			if (out_fd >= 0)
				close(out_fd);
			return (1);
		}
		if (pid == 0)
		{
			// リダイレクションを適用
			if (in_fd >= 0)
			{
				dup2(in_fd, STDIN_FILENO);
				close(in_fd);
			}
			else if (prev_pipe_read >= 0)
			{
				// 前のコマンドの出力を標準入力に接続
				dup2(prev_pipe_read, STDIN_FILENO);
				close(prev_pipe_read);
			}
			if (out_fd >= 0)
			{
				dup2(out_fd, STDOUT_FILENO);
				close(out_fd);
			}
			else if (has_next)
			{
				// 次のコマンドがある場合は標準出力をパイプに接続
				dup2(pipe_fd[1], STDOUT_FILENO);
				close(pipe_fd[0]);
				close(pipe_fd[1]);
			}
			// コマンドを実行
			if (is_builtin(args[0]))
				exit(execute_builtin(args, env));
			else
				exit(execute_external(args, env));
		}
		// リダイレクションを閉じる
		if (in_fd >= 0)
			close(in_fd);
		if (out_fd >= 0)
			close(out_fd);
		// 前のパイプを閉じる
		if (prev_pipe_read >= 0)
			close(prev_pipe_read);
		// 次のコマンドがある場合は書き込み側を閉じる
		if (has_next)
		{
			close(pipe_fd[1]);
			prev_pipe_read = pipe_fd[0];
		}
		else
		{
			// 最後のコマンドの終了を待つ
			waitpid(pid, &status, 0);
			free_array(args);
			break;
		}
		free_array(args);
	}
	// 中間のコマンドの終了を待つ
	while (waitpid(-1, &status, 0) > 0)
		;
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (1);
}

// 入力の解析と実行
void	parse_and_execute(char *input, t_env **env)
{
	t_token	*tok;
	char	**tokens;
	char	**args;
	int		in_fd;
	int		out_fd;
	int		saved_stdin;
	int		saved_stdout;

	// 入力が空の場合は何もしない
	if (!input || ft_strlen(input) == 0)
		return;
	// トークン化（クォート対応）
	tok = tokenize(input);
	if (!tok)
		return;
	// 環境変数展開とクォート除去を適用
	expand_and_remove_quotes(tok, *env);
	// トークンリストを文字列配列に変換
	tokens = tokens_to_array(tok);
	free_token(tok);
	if (!tokens)
		return;
	// パイプがある場合はパイプ処理を実行
	if (has_pipe(tokens))
	{
		g_signal = execute_pipe(tokens, env);
		free_array(tokens);
		return;
	}
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
		g_signal = execute_builtin(args, env);
	else
		g_signal = execute_external(args, env);
	// ファイルディスクリプタを復元
	restore_fds(saved_stdin, saved_stdout);
	// メモリを解放
	free_array(tokens);
	free(args);
}
