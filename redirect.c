#include "minishell.h"

// 入力ファイルを開く
int	open_input_file(char *filename)
{
	int	fd;

	if (!filename)
		return (-1);
	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		print_error(filename, NULL, strerror(errno));
		return (-1);
	}
	return (fd);
}

// 出力ファイルを開く
int	open_output_file(char *filename, int append)
{
	int	fd;
	int	flags;

	if (!filename)
		return (-1);
	if (append)
		flags = O_WRONLY | O_CREAT | O_APPEND;
	else
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	fd = open(filename, flags, 0644);
	if (fd < 0)
	{
		print_error(filename, NULL, strerror(errno));
		return (-1);
	}
	return (fd);
}

// heredocを処理
int	handle_heredoc(char *delimiter)
{
	int		pipe_fd[2];
	char	*line;
	char	*prompt;

	if (!delimiter)
		return (-1);
	if (pipe(pipe_fd) < 0)
	{
		print_error("pipe", NULL, strerror(errno));
		return (-1);
	}
	prompt = "> ";
	while (1)
	{
		line = readline(prompt);
		if (!line)
		{
			ft_putendl_fd("", STDOUT_FILENO);
			break;
		}
		if (ft_strncmp(line, delimiter, ft_strlen(delimiter) + 1) == 0)
		{
			free(line);
			break;
		}
		ft_putendl_fd(line, pipe_fd[1]);
		free(line);
	}
	close(pipe_fd[1]);
	return (pipe_fd[0]);
}

// リダイレクションを設定
int	setup_redirections(char **tokens, int *in_fd, int *out_fd)
{
	int	i;
	int	fd;

	if (!tokens || !in_fd || !out_fd)
		return (1);
	*in_fd = -1;
	*out_fd = -1;
	i = 0;
	while (tokens[i])
	{
		if (ft_strncmp(tokens[i], ">>", 3) == 0 && ft_strlen(tokens[i]) == 2)
		{
			if (!tokens[i + 1])
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
			if (!tokens[i + 1])
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
			if (!tokens[i + 1])
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
			if (!tokens[i + 1])
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

// ファイルディスクリプタを復元
void	restore_fds(int saved_stdin, int saved_stdout)
{
	if (saved_stdin >= 0)
	{
		dup2(saved_stdin, STDIN_FILENO);
		close(saved_stdin);
	}
	if (saved_stdout >= 0)
	{
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdout);
	}
}

