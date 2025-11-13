#include "minishell.h"

// オプションフラグの構造体
typedef struct s_cat_options
{
	int	show_ends;		// -e, -E: 行末に$を表示
	int	number_lines;	// -n: 行番号を表示
	int	number_nonblank; // -b: 空行以外に行番号を表示
	int	squeeze_blank;	// -s: 連続する空行を1行にまとめる
}	t_cat_options;

// 文字を出力（オプションに応じて処理）
static void	print_char_with_options(char c, t_cat_options *opts, int *line_num, int *is_newline)
{
	if (*is_newline)
	{
		*is_newline = 0;
		if (opts->number_lines || (opts->number_nonblank && c != '\n'))
		{
			ft_putnbr_fd(*line_num, STDOUT_FILENO);
			ft_putstr_fd("\t", STDOUT_FILENO);
			(*line_num)++;
		}
	}
	if (c == '\n')
	{
		if (opts->show_ends)
			ft_putchar_fd('$', STDOUT_FILENO);
		ft_putchar_fd('\n', STDOUT_FILENO);
		*is_newline = 1;
	}
	else
		ft_putchar_fd(c, STDOUT_FILENO);
}

// ファイルの内容を読み込んで標準出力に出力（オプション対応）
static int	read_and_print_file(char *filename, t_cat_options *opts)
{
	int		fd;
	char	buffer[4096];
	ssize_t	bytes_read;
	int		line_num;
	int		is_newline;
	int		i;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		print_error("cat", filename, strerror(errno));
		return (1);
	}
	line_num = 1;
	is_newline = 1;
	while (1)
	{
		bytes_read = read(fd, buffer, sizeof(buffer));
		if (bytes_read < 0)
		{
			print_error("cat", filename, strerror(errno));
			close(fd);
			return (1);
		}
		if (bytes_read == 0)
			break;
		i = 0;
		while (i < bytes_read)
		{
			print_char_with_options(buffer[i], opts, &line_num, &is_newline);
			i++;
		}
	}
	close(fd);
	return (0);
}

// 標準入力から読み込んで標準出力に出力（オプション対応）
static int	read_from_stdin(t_cat_options *opts)
{
	char	buffer[4096];
	ssize_t	bytes_read;
	int		line_num;
	int		is_newline;
	int		i;

	line_num = 1;
	is_newline = 1;
	while (1)
	{
		bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));
		if (bytes_read < 0)
		{
			print_error("cat", NULL, strerror(errno));
			return (1);
		}
		if (bytes_read == 0)
			break;
		i = 0;
		while (i < bytes_read)
		{
			print_char_with_options(buffer[i], opts, &line_num, &is_newline);
			i++;
		}
	}
	return (0);
}

// オプションを解析
static int	parse_options(char **args, t_cat_options *opts, int *arg_start)
{
	int	i;
	int	j;

	*opts = (t_cat_options){0, 0, 0, 0};
	*arg_start = 1;
	i = 1;
	while (args[i] && args[i][0] == '-')
	{
		if (ft_strlen(args[i]) == 1)
			break;
		j = 1;
		while (args[i][j])
		{
			if (args[i][j] == 'e' || args[i][j] == 'E')
				opts->show_ends = 1;
			else if (args[i][j] == 'n')
				opts->number_lines = 1;
			else if (args[i][j] == 'b')
				opts->number_nonblank = 1;
			else if (args[i][j] == 's')
				opts->squeeze_blank = 1;
			else
				return (0);
			j++;
		}
		i++;
		(*arg_start)++;
	}
	if (opts->number_nonblank)
		opts->number_lines = 0;
	return (1);
}

// cat コマンドの実装
int	ft_cat(char **args, t_env **env)
{
	int				i;
	int				exit_status;
	int				arg_start;
	t_cat_options	opts;

	(void)env;
	if (!args)
		return (1);
	// オプションを解析
	if (!parse_options(args, &opts, &arg_start))
	{
		print_error("cat", args[1], "invalid option");
		return (1);
	}
	// 引数がない場合は標準入力から読み込む
	if (!args[arg_start])
		return (read_from_stdin(&opts));
	// 複数のファイルを処理
	exit_status = 0;
	i = arg_start;
	while (args[i])
	{
		if (read_and_print_file(args[i], &opts) != 0)
			exit_status = 1;
		i++;
	}
	return (exit_status);
}

