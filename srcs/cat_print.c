/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cat_print.c                                        :+:      :+:    :+:   */
/*                                                    ft +:+         +:+      */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 05:25:00 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 05:25:00 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_char_with_options(char c,
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

int	process_file_content(int fd, char *filename,
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
