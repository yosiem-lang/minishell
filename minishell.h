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
# include "libft/libft.h"

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

// オプションフラグの構造体
typedef struct s_cat_options
{
	int	show_ends;		// -e, -E: 行末に$を表示
	int	number_lines;	// -n: 行番号を表示
	int	number_nonblank; // -b: 空行以外に行番号を表示
	int	squeeze_blank;	// -s: 連続する空行を1行にまとめる
}	t_cat_options;

typedef struct s_process_state
{
	int	line_num;	// 行番号 (line_num)
	int	is_newline; // 改行フラグ (is_newline)
}	t_process_state;

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
int		ft_cat(char **args, t_env **env);

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
t_token	*tokenize(char *line);
void	print_tokens(t_token *tok);
void	free_token(t_token *tok);

#endif
