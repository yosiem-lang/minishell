NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror -g -I/usr/include/readline
LDFLAGS = -lreadline -ltermcap

SRCDIR = .
LIBFTDIR = ./libft

# ソースファイル
SRCS = main.c builtins.c echo.c pwd.c env.c exit.c cd.c export.c unset.c env_utils.c utils.c signals.c tokenizer.c exec.c redirect.c cat.c

# オブジェクトファイル
OBJS = $(SRCS:%.c=%.o)

# ライブラリ
LIBFT = $(LIBFTDIR)/libft.a

# デフォルトターゲット
all: $(NAME)

# メインターゲット
$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LIBFT) $(LDFLAGS)

# オブジェクトファイルの作成
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# libftの作成
$(LIBFT):
	make -C $(LIBFTDIR)

# クリーンアップ
clean:
	rm -f $(OBJS)
	make -C $(LIBFTDIR) clean

# 完全クリーンアップ
fclean: clean
	rm -f $(NAME)
	make -C $(LIBFTDIR) fclean

# リビルド
re: fclean all

# フォニーターゲット
.PHONY: all clean fclean re
