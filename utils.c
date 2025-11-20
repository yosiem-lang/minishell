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
	if (!ft_isalpha(str[0]) && str[0] != '_')
		return (0);
	i = 1;
	while (str[i])
	{
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
		return ;
	expanded = get_expanded_string(*str, env);
	if (expanded)
	{
		free(*str);
		*str = expanded;
	}
}

// **引数4個:** result_ptr, env, var_start, var_end
static int	expand_regular_variable(char **result_ptr,
		t_env *env, char *var_start, char *var_end)
{
	char	*var_name;
	char	*var_value;
	int		len;

	len = var_end - var_start - 1;
	if (len <= 0)
		return (0);
	var_name = ft_substr(var_start + 1, 0, len);
	if (!var_name)
		return (-1);
	var_value = get_env_value(env, var_name);
	if (var_value)
		replace_variable(result_ptr, var_start, var_end, var_value);
	else
		replace_variable(result_ptr, var_start, var_end, "");
	free(var_name);
	return (1);
}

// **引数4個:** result_ptr, var_start, var_end, exit_status (g_signalの値)
static int	expand_special_variable(char **result_ptr,
		char *var_start, char *var_end, int exit_status)
{
	char	*var_value;

	if (var_end[0] != '?')
		return (0);
	var_value = ft_itoa(exit_status);
	if (!var_value)
		return (-1);
	replace_variable(result_ptr, var_start, var_end + 1, var_value);
	free(var_value);
	return (1);
}

// **引数4個:** result_ptr, env, var_start, g_signal
// 戻り値: >0: 処理成功 (1)、0: 処理なし、-1: メモリ/関数エラー
static int	process_next_variable(char **result_ptr,
		t_env *env, char *var_start, int g_signal)
{
	char	*var_end;
	int		ret;

	var_end = var_start + 1;
	while (var_end[0] && (ft_isalnum(var_end[0]) || var_end[0] == '_'))
		var_end++;
	ret = expand_regular_variable(result_ptr, env, var_start, var_end);
	if (ret != 0)
		return (ret);
	ret = expand_special_variable(result_ptr, var_start, var_end, g_signal);
	if (ret != 0)
		return (ret);
	return (0);
}

// **引数2個:** str, env
char	*get_expanded_string(char *str, t_env *env)
{
	char	*result;
	char	*var_start;
	int		ret;

	if (!str || !env)
		return (NULL);
	result = ft_strdup(str);
	if (!result)
		return (NULL);
	var_start = ft_strchr(result, '$');
	while (var_start)
	{
		ret = process_next_variable(&result, env, var_start, g_signal);
		if (ret == -1)
			return (free(result), NULL);
		if (ret == 1)
		{
			var_start = ft_strchr(result, '$');
			continue ;
		}
		var_start = ft_strchr(var_start + 1, '$');
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
		return ;
	len = ft_strlen(*str);
	value_len = ft_strlen(value);
	new_str = malloc(len - (end - start) + value_len + 1);
	if (!new_str)
		return ;
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
		return ;
	i = 0;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}
