/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cat_read.c                                         :+:      :+:    :+:   */
/*                                                    ft +:+         +:+      */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 05:25:00 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 05:25:00 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	read_and_print_file(char *filename, t_cat_options *opts)
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

int	read_from_stdin(t_cat_options *opts)
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
