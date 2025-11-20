#include "minishell.h"

/*-----------------------------------------------------------
** Utility functions
-----------------------------------------------------------*/

void	fatal_error(const char *msg)
{
	dprintf(STDERR_FILENO, "Fatal Error: %s\n", msg);
	exit(1);
}

void	assert_error(const char *msg)
{
	dprintf(STDERR_FILENO, "Assert Error: %s\n", msg);
	exit(255);
}

/*-----------------------------------------------------------
** Token constructor
-----------------------------------------------------------*/

t_token	*new_token(char *word, t_token_kind kind)
{
	t_token	*tok;

	tok = calloc(1, sizeof(*tok));
	if (tok == NULL)
		fatal_error("calloc");
	tok->word = word;
	tok->kind = kind;
	tok->next = NULL;
	return (tok);
}

/*-----------------------------------------------------------
** Basic character checks
-----------------------------------------------------------*/

bool	is_blank(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

bool	consume_blank(char **rest, char *line)
{
	while (*line && is_blank(*line))
		line++;
	*rest = line;
	return (*line != '\0');
}

bool	is_metacharacter(char c)
{
	return (c && strchr("|&;()<> \t\n", c));
}

bool	startswith(const char *s, const char *keyword)
{
	return (memcmp(s, keyword, strlen(keyword)) == 0);
}

/*-----------------------------------------------------------
** Operator checks
-----------------------------------------------------------*/

bool	is_operator(const char *s)
{
	static char *const	operators[] = {
		"||", "&", "&&", ";", ";;", "(", ")", "|", "\n", "<", ">", "<<", ">>"
	};
	size_t				i;

	i = 0;
	while (i < sizeof(operators) / sizeof(*operators))
	{
		if (startswith(s, operators[i]))
			return (true);
		i++;
	}
	return (false);
}

t_token	*operator(char **rest, char *line)
{
	static char *const	operators[] = {
		"||", "&", "&&", ";", ";;", "(", ")", "|", "\n", "<", ">", "<<", ">>"
	};
	size_t				i;
	char				*op;

	i = 0;
	while (i < sizeof(operators) / sizeof(*operators))
	{
		if (startswith(line, operators[i]))
		{
			op = strdup(operators[i]);
			if (op == NULL)
				fatal_error("strdup");
			*rest = line + strlen(op);
			return (new_token(op, TK_OP));
		}
		i++;
	}
	assert_error("Unexpected operator");
	return (NULL);
}

/*-----------------------------------------------------------
** Word token
-----------------------------------------------------------*/

#define SINGLE_QUOTE_CHAR '\''
#define DOUBLE_QUOTE_CHAR '\"'

bool	is_word(const char *s)
{
	return (*s && !is_metacharacter(*s));
}

/* **引数3個:** line (現在の開始位置), is_metacharacter,
  SINGLE_QUOTE_CHAR, DOUBLE_QUOTE_CHAR
注意: is_metacharacter, SINGLE_QUOTE_CHAR, DOUBLE_QUOTE_CHAR は
  関数外で定義されている定数/関数と仮定します。*/
static char	*scan_word_end(char *line)
{
	while (*line && !is_metacharacter(*line))
	{
		if (*line == SINGLE_QUOTE_CHAR)
		{
			line++;
			while (*line && *line != SINGLE_QUOTE_CHAR)
				line++;
			if (*line == '\0')
				fatal_error("Unclosed single quote");
			line++;
		}
		else if (*line == DOUBLE_QUOTE_CHAR)
		{
			line++;
			while (*line && *line != DOUBLE_QUOTE_CHAR)
				line++;
			if (*line == '\0')
				fatal_error("Unclosed double quote");
			line++;
		}
		else
			line++;
	}
	return (line);
}

// **引数2個:** rest (残りの行を返すポインタ), line (入力行の現在の開始位置)
t_token	*word(char **rest, char *line)
{
	const char	*start = line;
	char		*end;
	char		*word_str;

	end = scan_word_end(line);
	word_str = strndup(start, end - start);
	if (word_str == NULL)
		fatal_error("strndup");
	*rest = end;
	return (new_token(word_str, TK_WORD));
}

/*-----------------------------------------------------------
** Quote removal
-----------------------------------------------------------*/

void	append_char(char **s, char c)
{
	size_t	size;
	char	*new;

	size = 2;
	if (*s)
		size += strlen(*s);
	new = malloc(size);
	if (new == NULL)
		fatal_error("malloc");
	if (*s)
		strlcpy(new, *s, size);
	new[size - 2] = c;
	new[size - 1] = '\0';
	if (*s)
		free(*s);
	*s = new;
}

// 文字列を結合して最初の文字列を解放
static char	*strjoin_free(char *s1, char *s2)
{
	char	*result;

	if (!s1 && !s2)
		return (NULL);
	if (!s1)
		return (ft_strdup(s2));
	if (!s2)
		return (s1);
	result = ft_strjoin(s1, s2);
	free(s1);
	return (result);
}

// **引数4個:** p (現在位置へのポインタ), env, var_end, expanded_part_ptr
static int	extract_and_lookup_var(char **p,
		t_env *env, char *var_end, char **expanded_part_ptr)
{
	char	*var_start;
	int		len;
	char	*var_name;
	char	*var_value;

	var_start = *p;
	len = var_end - var_start - 1;
	if (len <= 0)
		return (0);
	var_name = ft_substr(var_start + 1, 0, len);
	if (!var_name)
		fatal_error("ft_substr");
	var_value = get_env_value(env, var_name);
	if (var_value)
		*expanded_part_ptr = ft_strdup(var_value);
	else
		*expanded_part_ptr = ft_strdup("");
	if (!*expanded_part_ptr)
	{
		free(var_name);
		fatal_error("ft_strdup");
	}
	free(var_name);
	return (1);
}

// **引数4個:** result (結果へのポインタ), p (現在位置へのポインタ), env, var_end (変数の終端)
static int	expand_variable_name(char **result,
		char **p, t_env *env, char *var_end)
{
	char	*expanded_part;
	char	*var_start;
	int		ret;

	var_start = *p;
	ret = extract_and_lookup_var(p, env, var_end, &expanded_part);
	if (ret == 0)
		return (0);
	*result = strjoin_free(*result, expanded_part);
	free(expanded_part);
	*p = var_end;
	return (1);
}

/* **引数4個:** result (結果へのポインタ), p (現在位置へのポインタ),
	var_end (変数の終端), *g_signal (グローバル変数)
Note: g_signal を引数として渡すか、グローバル変数を直接使用するかは規約によりますが、引数として渡すのが望ましいです。
ここでは g_signal を直接参照できないため、int *exit_status_ptr を使用し、
	呼び出し元で g_signal を渡すことを想定します。*/
static int	expand_special_variable(char **result,
		char **p, int *exit_status_ptr, char *var_end)
{
	char	*var_start;
	char	*var_value;

	var_start = *p;
	if (*(var_start + 1) != '?')
		return (0);
	var_value = ft_itoa(*exit_status_ptr);
	if (!var_value)
		fatal_error("ft_itoa");
	*result = strjoin_free(*result, var_value);
	free(var_value);
	*p = var_end + 1;
	return (1);
}

// **引数4個:** result, p (現在位置へのポインタ), env, g_signal
// p を進める処理（文字追加なし）をこの関数が行う
static int	handle_dollar_expansion(char **result,
		char **p, t_env *env, int g_signal)
{
	char	*var_start;
	char	*var_end;

	var_start = *p;
	var_end = var_start + 1;
	while (*var_end && (ft_isalnum(*var_end) || *var_end == '_'))
		var_end++;
	if (var_end - var_start > 1)
	{
		if (expand_variable_name(result, p, env, var_end))
			return (1);
	}
	else if (*var_end == '?')
	{
		if (expand_special_variable(result, p, &g_signal, var_end))
			return (1);
	}
	return (0);
}

// **引数4個:** result, p, env, quote_char
static void	expand_in_quotes(char **result,
		char **p, t_env *env, char quote_char)
{
	int	ret;

	(*p)++;
	while (**p && **p != quote_char)
	{
		if (quote_char == DOUBLE_QUOTE_CHAR && **p == '$')
		{
			ret = handle_dollar_expansion(result, p, env, g_signal);
			if (ret == 1)
				continue ;
		}
		append_char(result, **p);
		(*p)++;
	}
	if (**p == '\0')
	{
		if (quote_char == SINGLE_QUOTE_CHAR)
			fatal_error("Unclosed single quote");
		else
			fatal_error("Unclosed double quote");
	}
	(*p)++;
}

// **引数3個:** new_word (結果へのポインタ), p (走査ポインタへのポインタ)
static void	handle_single_quote(char **new_word, char **p)
{
	(*p)++;
	while (**p && **p != SINGLE_QUOTE_CHAR)
		append_char(new_word, *(*p)++);
	if (**p == '\0')
		fatal_error("Unclosed single quote");
	(*p)++;
}

// **引数4個:** new_word, var_start, var_end, env
static int	expand_regular_unquoted(char **new_word,
		char *var_start, char *var_end, t_env *env)
{
	char	*var_name;
	char	*var_value;
	int		len;

	len = var_end - var_start - 1;
	if (len <= 0)
		return (0);
	var_name = ft_substr(var_start + 1, 0, len);
	if (!var_name)
		fatal_error("ft_substr");
	var_value = get_env_value(env, var_name);
	if (var_value)
		*new_word = strjoin_free(*new_word, var_value);
	free(var_name);
	return (1);
}

// **引数4個:** new_word, p_ptr, var_end, exit_status
// p_ptr (char **p) はメイン関数内で *p = var_end + 1 のために必要
static int	handle_special_unquoted(char **new_word,
		char **p_ptr, char *var_end, int exit_status)
{
	char	*var_value;

	if (*var_end == '?')
	{
		var_value = ft_itoa(exit_status);
		if (var_value)
		{
			*new_word = strjoin_free(*new_word, var_value);
			free(var_value);
		}
		*p_ptr = var_end + 1;
		return (1);
	}
	return (0);
}

// **引数4個:** new_word, p, env, exit_status
static int	handle_unquoted_var(char **new_word,
		char **p, t_env *env, int exit_status)
{
	char	*var_start;
	char	*var_end;
	int		ret;

	var_start = *p;
	var_end = *p + 1;
	while (*var_end && (ft_isalnum(*var_end) || *var_end == '_'))
		var_end++;
	if (var_end - var_start > 1)
	{
		if (expand_regular_unquoted(new_word, var_start, var_end, env))
		{
			*p = var_end;
			return (1);
		}
	}
	ret = handle_special_unquoted(new_word, p, var_end, exit_status);
	return (ret);
}

// **引数2個:** new_word, p
static void	process_current_char(char **new_word, char **p)
{
	append_char(new_word, **p);
	(*p)++;
}

// **引数4個:** new_word (結果へのポインタ), p (走査ポインタへのポインタ), env, g_signal
static void	process_single_token(char **new_word,
		char **p, t_env *env, int g_signal)
{
	int	ret;

	while (**p)
	{
		if (**p == SINGLE_QUOTE_CHAR)
			handle_single_quote(new_word, p);
		else if (**p == DOUBLE_QUOTE_CHAR)
			expand_in_quotes(new_word, p, env, DOUBLE_QUOTE_CHAR);
		else if (**p == '$')
		{
			ret = handle_unquoted_var(new_word, p, env, g_signal);
			if (ret == 1)
				continue ;
			process_current_char(new_word, p);
		}
		else
			process_current_char(new_word, p);
	}
}

// **引数3個:** tok (トークンリスト), env (環境変数)
void	expand_and_remove_quotes(t_token *tok, t_env *env)
{
	char	*new_word;
	char	*p;
	char	**p_ptr;
	int		g_signal_val;

	g_signal_val = g_signal;
	while (tok && tok->kind != TK_EOF)
	{
		new_word = NULL;
		p = tok->word;
		p_ptr = &p;
		process_single_token(&new_word, p_ptr, env, g_signal_val);
		free(tok->word);
		tok->word = new_word;
		tok = tok->next;
	}
}

// **引数3個:** p_ptr (走査ポインタへのポインタ), new_word_ptr, quote_char
// 戻り値: 1: クォート処理を実行した, 0: 処理せず
static int	handle_quoted_segment(char **p_ptr,
		char **new_word_ptr, char quote_char)
{
	char	*p;
	char	*unclosed_error_msg;

	p = *p_ptr;
	if (*p != quote_char)
		return (0);
	p++;
	while (*p && *p != quote_char)
		append_char(new_word_ptr, *p++);
	if (*p == '\0')
	{
		if (quote_char == SINGLE_QUOTE_CHAR)
			unclosed_error_msg = "Unclosed single quote";
		else
			unclosed_error_msg = "Unclosed double quote";
		fatal_error(unclosed_error_msg);
	}
	p++;
	*p_ptr = p;
	return (1);
}

// クォート除去処理のメイン関数
// **引数2個:** word (処理対象の文字列), new_word_ptr (結果を格納するポインタ)
static void	remove_quotes_from_word(char *word, char **new_word_ptr)
{
	char	*p;

	p = word;
	while (*p)
	{
		if (handle_quoted_segment(&p, new_word_ptr, SINGLE_QUOTE_CHAR))
			continue ;
		if (handle_quoted_segment(&p, new_word_ptr, DOUBLE_QUOTE_CHAR))
			continue ;
		append_char(new_word_ptr, *p++);
	}
}

// クォート除去処理のメイン関数
void	quote_removal(t_token *tok)
{
	char	*new_word;

	while (tok && tok->kind != TK_EOF)
	{
		new_word = NULL;
		remove_quotes_from_word(tok->word, &new_word);
		free(tok->word);
		tok->word = new_word;
		tok = tok->next;
	}
}

/*-----------------------------------------------------------
** Main tokenizer
-----------------------------------------------------------*/

// **引数3個:** tok_ptr (現在のリストの末尾へのポインタ), line_ptr (入力文字列ポインタへのポインタ), line (現在の位置)
// 戻り値: 成功時に新しいトークンへのポインタを返す。エラー時にNULLを返す。空白消費時はNULLを返す。
static t_token	*tokenize_dispatcher(t_token *current_tail,
		char **line_ptr, char *line)
{
	t_token	*new_tok;

	if (is_blank(*line))
	{
		consume_blank(line_ptr, line);
		return (NULL);
	}
	else if (is_operator(line))
		new_tok = operator(line_ptr, line);
	else if (is_word(line))
		new_tok = word(line_ptr, line);
	else
	{
		assert_error("Unexpected Token");
		return (NULL);
	}
	current_tail->next = new_tok;
	return (new_tok);
}

t_token	*tokenize(char *line)
{
	t_token	head;
	t_token	*tok;
	t_token	*new_tail;

	head.next = NULL;
	tok = &head;
	while (*line)
	{
		*new_tail = tokenize_dispatcher(tok, &line, line);
		if (new_tail == NULL && !is_blank(*line))
			return (head.next);
		if (new_tail != NULL)
			tok = new_tail;
	}
	tok->next = new_token(NULL, TK_EOF);
	return (head.next);
}

/*-----------------------------------------------------------
** Debug print
-----------------------------------------------------------*/
void	print_tokens(t_token *tok)
{
	while (tok && tok->kind != TK_EOF)
	{
		printf("[%s]", tok->word);
		if (tok->kind == TK_OP)
			printf(" (OP)");
		printf("\n");
		tok = tok->next;
	}
	printf("(EOF)\n");
}

void	free_token(t_token *tok)
{
	t_token	*next;

	while (tok)
	{
		next = tok->next;
		free(tok->word);
		free(tok);
		tok = next;
	}
}

// /*-----------------------------------------------------------
// ** main for testing
// -----------------------------------------------------------*/

// int	main(void)
// {
// 	char line[256];

// 	while (1)
// 	{
// 		printf("minishell$ ");
// 		if (!fgets(line, sizeof(line), stdin))
// 			break;
// 		t_token *tok = tokenize(line);
// 		print_tokens(tok);
// 		free_token(tok);
// 	}
// 	return (0);
// }
