# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mkazuhik <mkazuhik@student.42tokyo.jp>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/22 04:54:28 by mkazuhik          #+#    #+#              #
#    Updated: 2025/11/23 03:37:18 by mkazuhik         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror -g -I/usr/include/readline -I. -Isrcs -Ilibft
LDFLAGS = -lreadline -ltermcap

SRCDIR = srcs
LIBFTDIR = ./libft

# ソースファイル
SRCS = $(SRCDIR)/main.c \
	$(SRCDIR)/builtins.c \
	$(SRCDIR)/echo.c \
	$(SRCDIR)/pwd.c \
	$(SRCDIR)/env.c \
	$(SRCDIR)/exit.c \
	$(SRCDIR)/cd.c \
	$(SRCDIR)/export.c \
	$(SRCDIR)/export_list.c \
	$(SRCDIR)/unset.c \
	$(SRCDIR)/signals.c \
	$(SRCDIR)/tokenizer.c \
	$(SRCDIR)/env_init.c \
	$(SRCDIR)/env_ops.c \
	$(SRCDIR)/env_array.c \
	$(SRCDIR)/utils_error.c \
	$(SRCDIR)/utils_expand.c \
	$(SRCDIR)/utils_memory.c \
	$(SRCDIR)/exec_path.c \
	$(SRCDIR)/exec_path_utils.c \
	$(SRCDIR)/exec_process.c \
	$(SRCDIR)/redirect_file.c \
	$(SRCDIR)/redirect_heredoc.c \
	$(SRCDIR)/redirect_setup.c \
	$(SRCDIR)/parse_args.c \
	$(SRCDIR)/fill_args.c \
	$(SRCDIR)/parse_pipe.c \
	$(SRCDIR)/pipe_redirections.c \
	$(SRCDIR)/pipe_exec.c \
	$(SRCDIR)/pipe_exec_utils.c \
	$(SRCDIR)/execute.c \
	$(SRCDIR)/tokenizer_utils.c \
	$(SRCDIR)/tokenizer_word.c \
	$(SRCDIR)/tokenizer_expand.c \
	$(SRCDIR)/tokenizer_expand2.c \
	$(SRCDIR)/tokenizer_expand3.c \
	$(SRCDIR)/tokenizer_expand4.c \
	$(SRCDIR)/tokenizer_quote.c \
	$(SRCDIR)/tokenizer_main.c

# オブジェクトファイル（ソースファイルと同じディレクトリ構造）
OBJS = $(SRCS:$(SRCDIR)/%.c=$(SRCDIR)/%.o)

# ライブラリ
LIBFT = $(LIBFTDIR)/libft.a

# デフォルトターゲット
all: $(NAME)

# メインターゲット
$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LIBFT) $(LDFLAGS)

# オブジェクトファイルの作成（ソースファイルと同じディレクトリ構造）
$(SRCDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# libftの作成
$(LIBFT):
	make -C $(LIBFTDIR)

# クリーンアップ
clean:
	rm -f $(OBJS)
	find $(SRCDIR) -name "*.o" -type f -delete
	make -C $(LIBFTDIR) clean

# 完全クリーンアップ
fclean: clean
	rm -f $(NAME)
	make -C $(LIBFTDIR) fclean

# リビルド
re: fclean all

# フォニーターゲット
.PHONY: all clean fclean re
