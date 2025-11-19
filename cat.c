#include "minishell.h"

// 文字を出力（オプションに応じて処理）
static void	print_char_with_options(char c,
		t_cat_options *opts, int *line_num, int *is_newline)
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

static int	process_file_content(int fd, char *filename,
		t_cat_options *opts, t_process_state *state)
{
	char	buffer[4096];
	ssize_t	bytes_read;
	int		i;

	while (1)
	{
		bytes_read = read(fd, buffer, sizeof(buffer));
		if (bytes_read < 0)
		{
			print_error("cat", filename, strerror(errno));
			return (1);
		}
		if (bytes_read == 0)
			break ;
		i = 0;
		while (i < bytes_read)
		{
			print_char_with_options(buffer[i], opts,
				&state->line_num, &state->is_newline);
			i++;
		}
	}
	return (0);
}

static int	read_and_print_file(char *filename, t_cat_options *opts)
{
	int				fd;
	int				status;
	t_process_state	state;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		print_error("cat", filename, strerror(errno));
		return (1);
	}
	state.line_num = 1;
	state.is_newline = 1;
	status = process_file_content(fd, filename, opts, &state);
	close(fd);
	return (status);
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
			break ;
		i = 0;
		while (i < bytes_read)
			print_char_with_options(buffer[i++], opts, &line_num, &is_newline);
	}
	return (0);
}

// 新しい関数: 単一のオプション文字列を解析する (引数2個)
static int	set_options_from_arg(char *arg, t_cat_options *opts)
{
	int	j;

	j = 1;
	while (arg[j])
	{
		if (arg[j] == 'e' || arg[j] == 'E')
			opts->show_ends = 1;
		else if (arg[j] == 'n')
			opts->number_lines = 1;
		else if (arg[j] == 'b')
			opts->number_nonblank = 1;
		else if (arg[j] == 's')
			opts->squeeze_blank = 1;
		else
			return (0);
		j++;
	}
	return (1);
}

// 元の関数 (引数3個: args, opts, arg_start)
static int	parse_options(char **args, t_cat_options *opts, int *arg_start)
{
	int	i;

	*opts = (t_cat_options){0, 0, 0, 0};
	*arg_start = 1;
	i = 1;
	while (args[i] && args[i][0] == '-')
	{
		if (ft_strlen(args[i]) == 1)
			break ;
		if (set_options_from_arg(args[i], opts) == 0)
			return (0);
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
	if (!parse_options(args, &opts, &arg_start))
	{
		print_error("cat", args[1], "invalid option");
		return (1);
	}
	if (!args[arg_start])
		return (read_from_stdin(&opts));
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
