/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 04:54:10 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/22 04:54:11 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// グローバル変数（シグナル用）
int	g_signal = 0;

// シグナルハンドラーを設定
void	setup_signal_handlers(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	sa_int.sa_handler = sigint_handler;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sigaction(SIGINT, &sa_int, NULL);
	sa_quit.sa_handler = sigquit_handler;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = 0;
	sigaction(SIGQUIT, &sa_quit, NULL);
}

// SIGINT ハンドラー (Ctrl+C)
void	sigint_handler(int sig)
{
	(void)sig;
	g_signal = 130;
	ft_putchar_fd('\n', STDOUT_FILENO);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

// SIGQUIT ハンドラー (Ctrl+\)
// 対話モードでは何もしない
void	sigquit_handler(int sig)
{
	(void)sig;
}
