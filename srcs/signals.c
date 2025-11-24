/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 04:54:10 by mkazuhik          #+#    #+#             */
/*   Updated: 2025/11/25 03:47:40 by mkazuhik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// グローバル変数（シグナル用）
int	g_signal = 0;

// シグナルハンドラーを設定（親プロセス用）
void	setup_signal_handlers(void)
{
	signal(SIGINT, sigint_handler);
	signal(SIGQUIT, SIG_IGN);
}

// SIGINT ハンドラー (Ctrl+C)
void	sigint_handler(int sig)
{
	(void)sig;
	g_signal = 130;
	ft_putchar_fd('\n', STDOUT_FILENO);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
}

// 子プロセス用のシグナル設定（パイプ実行時）
void	child_signal_setting(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}
