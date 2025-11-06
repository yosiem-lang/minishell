#include "minishell.h"

// エラーメッセージを出力
void	print_error(char *cmd, char *arg, char *error)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd(cmd, STDERR_FILENO);
	ft_putstr_fd(": ", STDERR_FILENO);
	if (arg)
	{
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
	}
	ft_putendl_fd(error, STDERR_FILENO);
}

// 有効な識別子かチェック
int	is_valid_identifier(char *str)
{
	int	i;

	if (!str || !str[0])
		return (0);
	// 最初の文字は英字またはアンダースコア
	if (!ft_isalpha(str[0]) && str[0] != '_')
		return (0);
	i = 1;
	while (str[i])
	{
		// 英数字またはアンダースコア
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

// 文字列が数字のみかチェック
int	ft_strisdigit(char *str)
{
	int	i;

	if (!str || !str[0])
		return (0);
	i = 0;
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

// 変数展開
void	expand_variables(char **str, t_env *env)
{
	char	*expanded;

	if (!str || !*str || !env)
		return;
	expanded = get_expanded_string(*str, env);
	if (expanded)
	{
		free(*str);
		*str = expanded;
	}
}

// 展開された文字列を取得
char	*get_expanded_string(char *str, t_env *env)
{
	char	*result;
	char	*var_start;
	char	*var_end;
	char	*var_name;
	char	*var_value;
	int		len;

	if (!str || !env)
		return (NULL);
	result = ft_strdup(str);
	if (!result)
		return (NULL);
	var_start = ft_strchr(result, '$');
	while (var_start)
	{
		var_end = var_start + 1;
		while (var_end[0] && (ft_isalnum(var_end[0]) || var_end[0] == '_'))
			var_end++;
		len = var_end - var_start - 1;
		if (len > 0)
		{
			var_name = ft_substr(var_start + 1, 0, len);
			if (var_name)
			{
				var_value = get_env_value(env, var_name);
				if (var_value)
				{
					// 変数を値に置換
					replace_variable(&result, var_start, var_end, var_value);
				}
				else
				{
					// 変数が存在しない場合は空文字に置換
					replace_variable(&result, var_start, var_end, "");
				}
				free(var_name);
			}
		}
		else if (var_end[0] == '?')
		{
			// $? の処理
			var_value = ft_itoa(g_signal);
			if (var_value)
			{
				replace_variable(&result, var_start, var_end + 1, var_value);
				free(var_value);
			}
		}
		var_start = ft_strchr(result, '$');
	}
	return (result);
}

// 変数を値に置換
void	replace_variable(char **str, char *start, char *end, char *value)
{
	char	*new_str;
	int		len;
	int		value_len;

	if (!str || !*str || !start || !end || !value)
		return;
	len = ft_strlen(*str);
	value_len = ft_strlen(value);
	new_str = malloc(len - (end - start) + value_len + 1);
	if (!new_str)
		return;
	ft_strlcpy(new_str, *str, start - *str + 1);
	ft_strlcat(new_str, value, ft_strlen(new_str) + ft_strlen(value) + 1);
	ft_strlcat(new_str, end, ft_strlen(new_str) + ft_strlen(end) + 1);
	free(*str);
	*str = new_str;
}

// 配列を解放
void	free_array(char **array)
{
	int	i;

	if (!array)
		return;
	i = 0;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}
