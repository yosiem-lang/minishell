/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 04:54:23 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/23 04:09:35 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/wait.h>
# include <signal.h>
# include <errno.h>
# include <fcntl.h>
# include <dirent.h>
# include <sys/stat.h>
# include <termios.h>
# include <curses.h>
# include <limits.h>
# include <stdbool.h>
# include <ctype.h>
# include "libft.h"

// グローバル変数（シグナル用）
extern int	g_signal;

/*-----------------------------------------------------------
** Token structure
-----------------------------------------------------------*/

typedef enum e_token_kind
{
	TK_WORD,
	TK_OP,
	TK_EOF
}	t_token_kind;

typedef struct s_token
{
	char			*word;
	t_token_kind	kind;
	struct s_token	*next;
}	t_token;

// tokensの走査範囲を定義する構造体
typedef struct s_token_range
{
	int	start_idx;
	int	end_idx;
}	t_token_range;

// t_fd_info 構造体 (内部に status を追加)
typedef struct s_fd_info
{
	int	in_fd;
	int	out_fd;
	int	prev_pipe_read;
	int	pipe_fd[2];
	int	status; // 🚨 New field to hold exit status 🚨
}	t_fd_info;

/* **引数4個:** tokens, args, env, fd_ptr 
(in_fd, out_fd, saved_stdin, saved_stdout)*/
typedef struct s_fds
{
	int	in_fd;
	int	out_fd;
	int	saved_stdin;
	int	saved_stdout;
}	t_fds;

// 環境変数管理用の構造体
typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

// コマンド解析用の構造体
typedef struct s_cmd
{
	char			**args;
	char			*input_file;
	char			*output_file;
	int				append_mode;
	struct s_cmd	*next;
}	t_cmd;

// ビルトインコマンドの関数ポインタ型
typedef int	(*t_builtin_func)(char **args, t_env **env);

// ビルトインコマンドの構造体
typedef struct s_builtin
{
	char			*name;
	t_builtin_func	func;
}	t_builtin;

typedef struct s_env_pair
{
	char	*key;
	char	*value;
}	t_env_pair;

// メイン関数
int		minishell_loop(t_env **env);
void	parse_and_execute(char *input, t_env **env);
int		execute_command(t_cmd *cmd, t_env **env);
int		is_builtin(char *cmd);
int		execute_builtin(char **args, t_env **env);

// ビルトインコマンド
int		ft_echo(char **args, t_env **env);
int		ft_cd(char **args, t_env **env);
int		ft_pwd(char **args, t_env **env);
int		ft_export(char **args, t_env **env);
int		print_export_list(t_env *env);
int		ft_unset(char **args, t_env **env);
int		ft_env(char **args, t_env **env);
int		ft_exit(char **args, t_env **env);

// 環境変数管理
t_env	*init_env(char **envp);
t_env	*create_env_node(char *key, char *value);
void	add_env_node(t_env **env, char *key, char *value);
char	*get_env_value(t_env *env, char *key);
void	update_env_value(t_env *env, char *key, char *value);
void	remove_env_node(t_env **env, char *key);
void	free_env_list(t_env *env);
char	**env_to_array(t_env *env);

// ユーティリティ関数
void	print_error(char *cmd, char *arg, char *error);
int		is_valid_identifier(char *str);
void	expand_variables(char **str, t_env *env);
void	expand_variables_with_quotes(char **str, t_env *env);
char	*get_expanded_string(char *str, t_env *env);
char	*get_expanded_string_with_quotes(char *str, t_env *env);
void	replace_variable(char **str, char *start, char *end, char *value);
void	free_array(char **array);
int		ft_strisdigit(char *str);

// 外部コマンド実行
int		execute_external(char **argv, t_env **env);
char	*find_executable(char *cmd, t_env *env);
char	*join_path(const char *dir, const char *cmd);
char	*search_path_dirs(char **path_dirs, char *cmd);

// リダイレクション処理
int		setup_redirections(char **tokens, int *in_fd, int *out_fd);
int		open_input_file(char *filename);
int		open_output_file(char *filename, int append);
int		handle_heredoc(char *delimiter);
void	restore_fds(int saved_stdin, int saved_stdout);

// シグナル処理
void	setup_signal_handlers(void);
void	sigint_handler(int sig);
void	sigquit_handler(int sig);
void	child_signal_setting(void);

/*-----------------------------------------------------------
** Tokenizer function prototypes
-----------------------------------------------------------*/

void	fatal_error(const char *msg) __attribute__((noreturn));
void	assert_error(const char *msg) __attribute__((noreturn));
t_token	*new_token(char *word, t_token_kind kind);
bool	is_blank(char c);
bool	consume_blank(char **rest, char *line);
bool	is_metacharacter(char c);
bool	is_operator(const char *s);
t_token	*operator(char **rest, char *line);
bool	is_word(const char *s);
t_token	*word(char **rest, char *line);
void	append_char(char **s, char c);
void	quote_removal(t_token *tok);
void	expand_and_remove_quotes(t_token *tok, t_env *env);
char	*strjoin_free(char *s1, char *s2);
int		expand_variable_name(char **result, char **p, t_env *env,
			char *var_end);
int		expand_special_variable(char **result, char **p, int *exit_status_ptr,
			char *var_end);
int		handle_dollar_expansion(char **result, char **p, t_env *env,
			int g_signal);
void	expand_in_quotes(char **result, char **p, t_env *env, char quote_char);
void	handle_single_quote(char **new_word, char **p);
int		expand_regular_unquoted(char **new_word, char *var_start,
			char *var_end, t_env *env);
int		handle_special_unquoted(char **new_word, char **p_ptr,
			char *var_end, int exit_status);
int		handle_unquoted_var(char **new_word, char **p, t_env *env,
			int exit_status);
void	process_current_char(char **new_word, char **p);
void	process_single_token(char **new_word, char **p, t_env *env,
			int g_signal);
int		handle_quoted_segment(char **p_ptr, char **new_word_ptr,
			char quote_char);
int		remove_quotes_from_word(char *word, char **new_word_ptr);
t_token	*tokenize(char *line);
void	print_tokens(t_token *tok);
void	free_token(t_token *tok);

/*-----------------------------------------------------------
** Main.c function prototypes
-----------------------------------------------------------*/

char	**tokens_to_array(t_token *tok);
int		fill_token_array(t_token *tok, char **array, int count);
char	**extract_args(char **tokens);
int		count_command_args(char **tokens);
int		fill_command_args(char **tokens, char **args, int count);
int		is_redirection(char *token);
int		has_pipe(char **tokens);
char	**get_pipe_command(char **tokens, int *start_idx);
int		count_command_args_in_pipe(char **tokens, int start_i);
int		fill_pipe_command_args(char **tokens,
			char **args, int *start_idx_ptr, int count);
int		handle_heredoc_redirection(char **tokens,
			t_token_range range, int *in_fd, int *current_idx_ptr);
int		handle_file_input_redirection(char **tokens,
			t_token_range range, int *in_fd, int *current_idx_ptr);
int		handle_input_redirections(char **tokens,
			t_token_range range, int *in_fd, int *current_idx_ptr);
int		handle_output_redirections(char **tokens,
			t_token_range range, int *out_fd, int *current_idx_ptr);
int		setup_pipe_redirections(char **tokens,
			t_token_range range, int *in_fd, int *out_fd);
void	find_pipe_segment(char **tokens,
			int *start_idx_ptr, t_token_range *range_ptr, int *has_next_ptr);
void	execute_child_process_pipe(char **args,
			t_env **env, t_fd_info fd_info, int has_next);
void	cleanup_parent_fds_and_pipe(t_fd_info *fdi_ptr, int has_next);
pid_t	pipe_loop_segment(char **tokens,
			t_env **env, t_fd_info *fdi_ptr, int *start_idx_ptr);
int		execute_pipe(char **tokens, t_env **env);
int		handle_tokenization_and_args(char *input,
			t_env **env, char ***tokens_ptr, char ***args_ptr);
int		handle_redirection_and_execution(char **tokens,
			char **args, t_env **env, t_fds *fd_ptr);
void	cleanup_and_exit_single(char **tokens,
			char **args, int saved_stdin, int saved_stdout);
void	execute_single_command(char **tokens,
			char **args, t_env **env, t_fds *fds_ptr);

#endif
