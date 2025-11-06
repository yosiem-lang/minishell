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
	static char *const operators[] = {
		"||", "&", "&&", ";", ";;", "(", ")", "|", "\n", "<", ">", "<<", ">>"
	};
	size_t i = 0;

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
	static char *const operators[] = {
		"||", "&", "&&", ";", ";;", "(", ")", "|", "\n", "<", ">", "<<", ">>"
	};
	size_t i = 0;
	char *op;

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

t_token	*word(char **rest, char *line)
{
	const char	*start = line;
	char		*word;

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
	word = strndup(start, line - start);
	if (word == NULL)
		fatal_error("strndup");
	*rest = line;
	return (new_token(word, TK_WORD));
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

void	quote_removal(t_token *tok)
{
	while (tok && tok->kind != TK_EOF)
	{
		char *new_word = NULL;
		char *p = tok->word;

		while (*p)
		{
			if (*p == SINGLE_QUOTE_CHAR)
			{
				p++;
				while (*p && *p != SINGLE_QUOTE_CHAR)
					append_char(&new_word, *p++);
				if (*p == '\0')
					fatal_error("Unclosed single quote");
				p++;
			}
			else if (*p == DOUBLE_QUOTE_CHAR)
			{
				p++;
				while (*p && *p != DOUBLE_QUOTE_CHAR)
					append_char(&new_word, *p++);
				if (*p == '\0')
					fatal_error("Unclosed double quote");
				p++;
			}
			else
				append_char(&new_word, *p++);
		}
		free(tok->word);
		tok->word = new_word;
		tok = tok->next;
	}
}

/*-----------------------------------------------------------
** Main tokenizer
-----------------------------------------------------------*/

t_token	*tokenize(char *line)
{
	t_token	head;
	t_token	*tok;

	head.next = NULL;
	tok = &head;
	while (*line)
	{
		if (is_blank(*line))
		{
			consume_blank(&line, line);
			continue;
		}
		else if (is_operator(line))
			tok = tok->next = operator(&line, line);
		else if (is_word(line))
			tok = tok->next = word(&line, line);
		else
			assert_error("Unexpected Token");
	}
	tok->next = new_token(NULL, TK_EOF);
	quote_removal(head.next);
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
