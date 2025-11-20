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

// **引数3個:** pipe_fd_ptr
static int	heredoc_setup(int *pipe_fd_ptr)
{
	if (pipe(pipe_fd_ptr) < 0)
	{
		print_error("pipe", NULL, strerror(errno));
		return (-1);
	}
	return (0);
}

// **引数4個:** delimiter, write_fd
static int	heredoc_input_loop(char *delimiter, int write_fd)
{
	char	*line;
	char	*prompt;

	if (!delimiter || write_fd < 0)
		return (1);
	prompt = "> ";
	while (1)
	{
		line = readline(prompt);
		if (!line)
		{
			ft_putendl_fd("", STDOUT_FILENO);
			break ;
		}
		if (ft_strncmp(line, delimiter, ft_strlen(delimiter) + 1) == 0)
		{
			free(line);
			break ;
		}
		ft_putendl_fd(line, write_fd);
		free(line);
	}
	close(write_fd);
	return (0);
}

int	handle_heredoc(char *delimiter)
{
	int	pipe_fd[2];

	if (!delimiter)
		return (-1);
	if (heredoc_setup(pipe_fd) < 0)
		return (-1);
	if (heredoc_input_loop(delimiter, pipe_fd[1]) != 0)
	{
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		return (-1);
	}
	return (pipe_fd[0]);
}

// **引数3個:** tokens, in_fd, current_idx_ptr
static int	handle_heredoc_redirection(char **tokens,
		int *in_fd, int *current_idx_ptr)
{
	int	fd;
	int	i;

	i = *current_idx_ptr;
	if (ft_strncmp(tokens[i], "<<", 3) == 0 && ft_strlen(tokens[i]) == 2)
	{
		if (!tokens[i + 1])
		{
			print_error("syntax error", NULL, "unexpected end of file");
			return (-1);
		}
		if (*in_fd >= 0)
			close(*in_fd);
		fd = handle_heredoc(tokens[i + 1]);
		if (fd < 0)
			return (-1);
		*in_fd = fd;
		*current_idx_ptr += 2;
		return (1);
	}
	return (0);
}

// **引数3個:** tokens, in_fd, current_idx_ptr
static int	handle_file_input_redirection(char **tokens,
		int *in_fd, int *current_idx_ptr)
{
	int	fd;
	int	i;

	i = *current_idx_ptr;
	if (ft_strncmp(tokens[i], "<", 2) == 0 && ft_strlen(tokens[i]) == 1)
	{
		if (!tokens[i + 1])
		{
			print_error("syntax error", NULL, "unexpected end of file");
			return (-1);
		}
		if (*in_fd >= 0)
			close(*in_fd);
		fd = open_input_file(tokens[i + 1]);
		if (fd < 0)
			return (-1);
		*in_fd = fd;
		*current_idx_ptr += 2;
		return (1);
	}
	return (0);
}

// **引数3個:** tokens, in_fd, current_idx_ptr
static int	handle_input_redirections(char **tokens,
		int *in_fd, int *current_idx_ptr)
{
	int	ret;

	ret = handle_heredoc_redirection(tokens, in_fd, current_idx_ptr);
	if (ret != 0)
		return (ret);
	ret = handle_file_input_redirection(tokens, in_fd, current_idx_ptr);
	return (ret);
}

// **引数3個:** tokens, out_fd, current_idx_ptr
static int	handle_output_redirections(char **tokens,
		int *out_fd, int *current_idx_ptr)
{
	int	fd;
	int	append_mode;
	int	i;

	i = *current_idx_ptr;
	if ((ft_strncmp(tokens[i], ">>", 3) == 0 && ft_strlen(tokens[i]) == 2)
		|| (ft_strncmp(tokens[i], ">", 2) == 0 && ft_strlen(tokens[i]) == 1))
	{
		if (!tokens[i + 1])
		{
			print_error("syntax error", NULL, "unexpected end of file");
			return (-1);
		}
		if (*out_fd >= 0)
			close(*out_fd);
		append_mode = (ft_strncmp(tokens[i], ">>", 3) == 0);
		fd = open_output_file(tokens[i + 1], append_mode);
		if (fd < 0)
			return (-1);
		*out_fd = fd;
		*current_idx_ptr += 2;
		return (1);
	}
	return (0);
}

// **引数3個:** tokens, in_fd, out_fd
int	setup_redirections(char **tokens, int *in_fd, int *out_fd)
{
	int	i;
	int	ret;

	if (!tokens || !in_fd || !out_fd)
		return (1);
	*in_fd = -1;
	*out_fd = -1;
	i = 0;
	while (tokens[i])
	{
		ret = handle_input_redirections(tokens, in_fd, &i);
		if (ret == -1)
			return (1);
		if (ret == 1)
			continue ;
		ret = handle_output_redirections(tokens, out_fd, &i);
		if (ret == -1)
			return (1);
		if (ret == 1)
			continue ;
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
