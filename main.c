#include "minishell.h"

// メイン関数
int	main(int argc, char **argv, char **envp)
{
	t_env	*env;

	(void)argc;
	(void)argv;
	setup_signal_handlers();
	env = init_env(envp);
	if (!env)
	{
		ft_putendl_fd("minishell: failed to initialize environment",
			STDERR_FILENO);
		return (1);
	}
	minishell_loop(&env);
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
		input = readline(prompt);
		if (!input)
		{
			ft_putendl_fd("exit", STDOUT_FILENO);
			break ;
		}
		if (ft_strlen(input) > 0)
			add_history(input);
		parse_and_execute(input, env);
		free(input);
	}
	return (0);
}

// トークンリストを文字列配列に変換 (メイン関数)
static char	**tokens_to_array(t_token *tok)
{
	char	**array;
	int		count;
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
	if (fill_token_array(tok, array, count)
		!= 0)
	{
		free(array);
		return (NULL);
	}
	return (array);
}

// 配列にトークン文字列を格納し、エラー時に解放するヘルパー関数
// 引数: tok (トークンリストの先頭), array (割り当てられた配列), count (要素数) -> 引数3個
static int	fill_token_array(t_token *tok, char **array, int count)
{
	int		i;
	t_token	*current;

	i = 0;
	current = tok;
	while (current && current->kind != TK_EOF)
	{
		array[i] = ft_strdup(current->word);
		if (!array[i])
		{
			while (i > 0)
				free(array[--i]);
			return (1);
		}
		i++;
		current = current->next;
	}
	array[i] = NULL;
	if (i != count)
	{
		while (i > 0)
			free(array[--i]);
		return (1);
	}
	return (0);
}

// コマンド引数とリダイレクションを分離 (メイン関数)
static char	**extract_args(char **tokens)
{
	char	**args;
	int		count;

	count = count_command_args(tokens);
	if (count == 0)
		return (NULL);
	args = malloc(sizeof(char *) * (count + 1));
	if (!args)
		return (NULL);
	if (fill_command_args(tokens, args, count) != 0)
	{
		free(args);
		return (NULL);
	}
	return (args);
}

// 純粋なコマンド引数の数をカウントする
// 引数: tokens (トークン文字列配列) -> 引数1個
static int	count_command_args(char **tokens)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (tokens[i])
	{
		if (is_redirection(tokens[i]))
		{
			i += 2;
			continue ;
		}
		if (ft_strncmp(tokens[i], "|", 2) == 0)
			break ;
		count++;
		i++;
	}
	return (count);
}

// **引数4個:** tokens, args, current_j_ptr, token_idx
static int	copy_and_check_arg(char **tokens,
		char **args, int *current_j_ptr, int token_idx)
{
	int	current_j;

	current_j = *current_j_ptr;
	args[current_j] = ft_strdup(tokens[token_idx]);
	if (!args[current_j])
	{
		while (current_j > 0)
			free(args[--current_j]);
		return (1);
	}
	*current_j_ptr = current_j + 1;
	return (0);
}

// **引数3個:** args, j (最終的な要素数), count (期待される要素数)
static int	check_and_cleanup_final(char **args, int j, int count)
{
	if (j != count)
	{
		while (j > 0)
			free(args[--j]);
		return (1);
	}
	return (0);
}

// コマンド引数を割り当てられた配列に格納する
// **引数3個:** tokens (入力トークン), args (出力配列), count (要素数)
static int	fill_command_args(char **tokens, char **args, int count)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (tokens[i] && j < count)
	{
		if (is_redirection(tokens[i]))
		{
			i += 2;
			continue ;
		}
		if (ft_strncmp(tokens[i], "|", 2) == 0)
			break ;
		if (copy_and_check_arg(tokens, args, &j, i) != 0)
			return (1);
		i++;
	}
	args[j] = NULL;
	return (check_and_cleanup_final(args, j, count));
}

// トークンがリダイレクションオペレータであるか判定する
// 引数: token (チェック対象の文字列) -> 引数1個
static int	is_redirection(char *token)
{
	return (ft_strncmp(token, "<", 2) == 0
		|| ft_strncmp(token, ">", 2) == 0
		|| ft_strncmp(token, ">>", 3) == 0
		|| ft_strncmp(token, "<<", 3) == 0);
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

// パイプで分割されたコマンドの引数を取得 (メイン関数)
// 引数: tokens (トークン配列), start_idx (開始インデックスへのポインタ) -> 引数2個
static char	**get_pipe_command(char **tokens, int *start_idx)
{
	char	**args;
	int		count;

	count = count_command_args_in_pipe(tokens, *start_idx);
	if (count == 0)
		return (NULL);
	args = malloc(sizeof(char *) * (count + 1));
	if (!args)
		return (NULL);
	if (fill_pipe_command_args(tokens, args, start_idx, count) != 0)
	{
		free(args);
		return (NULL);
	}
	return (args);
}

// パイプまでのコマンド引数の数をカウントする
// 引数: tokens (トークン配列), start_i (開始インデックス) -> 引数2個
static int	count_command_args_in_pipe(char **tokens, int start_i)
{
	int	i;
	int	count;

	i = start_i;
	count = 0;
	while (tokens[i] && ft_strncmp(tokens[i], "|", 2) != 0)
	{
		if (is_redirection(tokens[i]))
		{
			i += 2;
			continue ;
		}
		count++;
		i++;
	}
	return (count);
}

// **引数3個:** tokens, args, current_j_ptr
// token_idxは不要。tokens[current_j_ptrが指す次の要素]をコピーするため。
static int	copy_and_cleanup_arg(char **tokens,
		char **args, int *current_j_ptr, int token_idx)
{
	int	current_j;

	current_j = *current_j_ptr;
	args[current_j] = ft_strdup(tokens[token_idx]);
	if (!args[current_j])
	{
		while (current_j > 0)
			free(args[--current_j]);
		return (1);
	}
	*current_j_ptr = current_j + 1;
	return (0);
}

// コマンド引数を割り当てられた配列に格納する
static int	fill_pipe_command_args(char **tokens,
		char **args, int *start_idx_ptr, int count)
{
	int	i;
	int	j;

	i = *start_idx_ptr;
	j = 0;
	while (tokens[i] && ft_strncmp(tokens[i], "|", 2) != 0)
	{
		if (is_redirection(tokens[i]))
		{
			i += 2;
			continue ;
		}
		if (copy_and_cleanup_arg(tokens, args, &j, i) != 0)
			return (1);
		i++;
	}
	args[j] = NULL;
	if (tokens[i] && ft_strncmp(tokens[i], "|", 2) == 0)
		*start_idx_ptr = i + 1;
	else
		*start_idx_ptr = i;
	return (0);
}

// **引数4個:** tokens, range, in_fd, current_idx_ptr
static int	handle_heredoc_redirection(char **tokens,
		t_token_range range, int *in_fd, int *current_idx_ptr)
{
	int	fd;
	int	i;

	i = *current_idx_ptr;
	if (ft_strncmp(tokens[i], "<<", 3) == 0 && ft_strlen(tokens[i]) == 2)
	{
		if (!tokens[i + 1] || i + 1 >= range.end_idx)
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
		*current_idx_ptr += 2;
		return (1);
	}
	return (0);
}

// **引数4個:** tokens, range, in_fd, current_idx_ptr
static int	handle_file_input_redirection(char **tokens,
		t_token_range range, int *in_fd, int *current_idx_ptr)
{
	int	fd;
	int	i;

	i = *current_idx_ptr;
	if (ft_strncmp(tokens[i], "<", 2) == 0 && ft_strlen(tokens[i]) == 1)
	{
		if (!tokens[i + 1] || i + 1 >= range.end_idx)
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
		*current_idx_ptr += 2;
		return (1);
	}
	return (0);
}

// **引数4個:** tokens, range, in_fd, current_idx_ptr
static int	handle_input_redirections(char **tokens,
		t_token_range range, int *in_fd, int *current_idx_ptr)
{
	int	ret;

	ret = handle_heredoc_redirection(tokens, range, in_fd, current_idx_ptr);
	if (ret != 0)
		return (ret);
	ret = handle_file_input_redirection(tokens, range, in_fd, current_idx_ptr);
	return (ret);
}

// **引数4個:** tokens, range, out_fd, 現在のインデックスへのポインタ
static int	handle_output_redirections(char **tokens,
		t_token_range range, int *out_fd, int *current_idx_ptr)
{
	int	fd;
	int	append_mode;
	int	i;

	i = *current_idx_ptr;
	if ((ft_strncmp(tokens[i], ">>", 3) == 0 && ft_strlen(tokens[i]) == 2)
		|| (ft_strncmp(tokens[i], ">", 2) == 0 && ft_strlen(tokens[i]) == 1))
	{
		if (!tokens[i + 1] || i + 1 >= range.end_idx)
		{
			print_error("syntax error", NULL, "unexpected end of file");
			return (1);
		}
		if (*out_fd >= 0)
			close(*out_fd);
		append_mode = (ft_strncmp(tokens[i], ">>", 3) == 0);
		fd = open_output_file(tokens[i + 1], append_mode);
		if (fd < 0)
			return (1);
		*out_fd = fd;
		*current_idx_ptr += 2;
		return (1);
	}
	return (0);
}

// **引数4個:** tokens, range, in_fd, out_fd
static int	setup_pipe_redirections(char **tokens,
		t_token_range range, int *in_fd, int *out_fd)
{
	int	i;

	if (!tokens || !in_fd || !out_fd)
		return (1);
	*in_fd = -1;
	*out_fd = -1;
	i = range.start_idx;
	while (i < range.end_idx && tokens[i])
	{
		if (handle_input_redirections(tokens, range, in_fd, &i))
		{
			if (*in_fd < 0)
				return (1);
			continue ;
		}
		else if (handle_output_redirections(tokens, range, out_fd, &i))
		{
			if (*out_fd < 0)
				return (1);
			continue ;
		}
		else
			i++;
	}
	return (0);
}

// **引数4個:** tokens, start_idx_ptr, range_ptr, has_next_ptr
static void	find_pipe_segment(char **tokens,
		int *start_idx_ptr, t_token_range *range_ptr, int *has_next_ptr)
{
	int	end_idx;

	range_ptr->start_idx = *start_idx_ptr;
	end_idx = *start_idx_ptr;
	while (tokens[end_idx] && ft_strncmp(tokens[end_idx], "|", 2) != 0)
		end_idx++;
	range_ptr->end_idx = end_idx;
	*has_next_ptr = (tokens[end_idx] != NULL);
}

// **引数4個:** has_next, pipe_fd, in_fd, out_fd
static int	handle_pipe_creation(int has_next,
		int *pipe_fd, int in_fd, int out_fd)
{
	if (has_next)
	{
		if (pipe(pipe_fd) < 0)
		{
			print_error("pipe", NULL, strerror(errno));
			if (in_fd >= 0)
				close(in_fd);
			if (out_fd >= 0)
				close(out_fd);
			return (1);
		}
	}
	return (0);
}

static void	execute_child_process(char **args,
		t_env **env, t_fd_info fd_info, int has_next)
{
	if (fd_info.in_fd >= 0)
	{
		dup2(fd_info.in_fd, STDIN_FILENO);
		close(fd_info.in_fd);
	}
	else if (fd_info.prev_pipe_read >= 0)
	{
		dup2(fd_info.prev_pipe_read, STDIN_FILENO);
		close(fd_info.prev_pipe_read);
	}
	if (fd_info.out_fd >= 0)
	{
		dup2(fd_info.out_fd, STDOUT_FILENO);
		close(fd_info.out_fd);
	}
	else if (has_next)
	{
		dup2(fd_info.pipe_fd[1], STDOUT_FILENO);
		close(fd_info.pipe_fd[0]);
		close(fd_info.pipe_fd[1]);
	}
	if (is_builtin(args[0]))
		exit(execute_builtin(args, env));
	else
		exit(execute_external(args, env));
}

// **引数2個:** fd_info_ptr, has_next
static void	cleanup_parent_fds_and_pipe(t_fd_info *fdi_ptr, int has_next)
{
	if (fdi_ptr->in_fd >= 0)
		close(fdi_ptr->in_fd);
	if (fdi_ptr->out_fd >= 0)
		close(fdi_ptr->out_fd);
	if (fdi_ptr->prev_pipe_read >= 0)
		close(fdi_ptr->prev_pipe_read);
	if (has_next)
	{
		close(fdi_ptr->pipe_fd[1]);
		fdi_ptr->prev_pipe_read = fdi_ptr->pipe_fd[0];
	}
	else
		fdi_ptr->prev_pipe_read = -1;
}

// **引数4個:** tokens, env, fdi_ptr, start_idx_ptr
static pid_t	pipe_loop_segment(char **tokens,
		t_env **env, t_fd_info *fdi_ptr, int *start_idx_ptr)
{
	char			**args;
	t_token_range	range;
	int				has_next;
	pid_t			pid;

	find_pipe_segment(tokens, start_idx_ptr, &range, &has_next);
	args = get_pipe_command(tokens, start_idx_ptr);
	if (!args || !args[0])
		return (0);
	pid = fork();
	if (pid == 0)
		execute_child_process(args, env, *fdi_ptr, has_next);
	cleanup_parent_fds_and_pipe(fdi_ptr, has_next);
	if (!has_next)
	{
		waitpid(pid, &fdi_ptr->status, 0);
		return (pid);
	}
	free_array(args);
	return (pid);
}

// パイプ処理を実行
static int	execute_pipe(char **tokens, t_env **env)
{
	t_fd_info		fdi;
	t_token_range	range;
	int				status;
	int				start_idx;
	pid_t			last_pid;

	fdi.prev_pipe_read = -1;
	start_idx = 0;
	while (1)
	{
		last_pid = pipe_loop_segment(tokens, env, &fdi, &start_idx);
		if (last_pid == 0)
			break ;
		if (last_pid == -1)
			return (1);
		if (fdi.prev_pipe_read == -1)
			break ;
	}
	while (waitpid(-1, &status, 0) > 0)
		;
	if (WIFEXITED(fdi.status))
		return (WEXITSTATUS(fdi.status));
	return (1);
}

// **引数4個:** input, env, tokens_ptr, args_ptr
// tokens_ptrとargs_ptrは結果を格納するためのポインタ
static int	handle_tokenization_and_args(char *input,
		t_env **env, char ***tokens_ptr, char ***args_ptr)
{
	t_token	*tok;
	char	**tokens;

	tok = tokenize(input);
	if (!tok)
		return (-1);
	expand_and_remove_quotes(tok, *env);
	tokens = tokens_to_array(tok);
	free_token(tok);
	if (!tokens)
		return (-1);
	*tokens_ptr = tokens;
	if (has_pipe(tokens))
		return (1);
	*args_ptr = extract_args(tokens);
	if (!*args_ptr || !(*args_ptr)[0])
	{
		free_array(tokens);
		if (*args_ptr)
			free(*args_ptr);
		return (-1);
	}
	return (0);
}

static int	handle_redirection_and_execution(char **tokens,
		char **args, t_env **env, t_fds *fd_ptr)
{
	int	exit_status;

	if (setup_redirections(tokens, &fd_ptr->in_fd, &fd_ptr->out_fd) != 0)
		return (-1);
	fd_ptr->saved_stdin = dup(STDIN_FILENO);
	fd_ptr->saved_stdout = dup(STDOUT_FILENO);
	if (fd_ptr->in_fd >= 0)
	{
		dup2(fd_ptr->in_fd, STDIN_FILENO);
		close(fd_ptr->in_fd);
	}
	if (fd_ptr->out_fd >= 0)
	{
		dup2(fd_ptr->out_fd, STDOUT_FILENO);
		close(fd_ptr->out_fd);
	}
	if (is_builtin(args[0]))
		exit_status = execute_builtin(args, env);
	else
		exit_status = execute_external(args, env);
	return (exit_status);
}

// **引数4個:** tokens, args, saved_stdin, saved_stdout
static void	cleanup_and_exit_single(char **tokens,
		char **args, int saved_stdin, int saved_stdout)
{
	restore_fds(saved_stdin, saved_stdout);
	free_array(tokens);
	free(args);
}

// **引数4個:** tokens, args, env, fds_ptr
// fds_ptr は t_fds *
static void	execute_single_command(char **tokens,
		char **args, t_env **env, t_fds *fds_ptr)
{
	int	exit_status;

	exit_status = handle_redirection_and_execution(tokens, args, env, fds_ptr);
	if (exit_status == -1)
	{
		free_array(tokens);
		free(args);
		g_signal = 1;
		return ;
	}
	cleanup_and_exit_single(tokens, args,
		fds_ptr->saved_stdin, fds_ptr->saved_stdout);
	g_signal = exit_status;
}

// 入力の解析と実行
void	parse_and_execute(char *input, t_env **env)
{
	char	**tokens;
	char	**args;
	t_fds	fds;
	int		ret;

	tokens = NULL;
	args = NULL;
	if (!input || ft_strlen(input) == 0)
		return ;
	ret = handle_tokenization_and_args(input, env, &tokens, &args);
	if (ret == -1)
		return ;
	if (ret == 1)
	{
		g_signal = execute_pipe(tokens, env);
		free_array(tokens);
		return ;
	}
	execute_single_command(tokens, args, env, &fds);
}
