#include "minishell.h"

static char	*join_path(const char *dir, const char *cmd)
{
    char    *tmp;
    char    *full;

    tmp = ft_strjoin(dir, "/");
    if (!tmp)
        return (NULL);
    full = ft_strjoin(tmp, cmd);
    free(tmp);
    return (full);
}

char	*find_executable(char *cmd, t_env *env)
{
    char    *path_env;
    char    **paths;
    char    *full;
    int     i;

    if (!cmd || !*cmd)
        return (NULL);
    if (ft_strchr(cmd, '/'))
        return (ft_strdup(cmd));
    path_env = get_env_value(env, "PATH");
    if (!path_env)
        return (NULL);
    paths = ft_split(path_env, ':');
    if (!paths)
        return (NULL);
    i = 0;
    while (paths[i])
    {
        full = join_path(paths[i], cmd);
        if (full && access(full, X_OK) == 0)
        {
            free_array(paths);
            return (full);
        }
        free(full);
        i++;
    }
    free_array(paths);
    return (NULL);
}

int	execute_external(char **argv, t_env **env)
{
    pid_t   pid;
    int     status;
    char    *exec_path;
    char    **envp;

    if (!argv || !argv[0])
        return (1);
    exec_path = find_executable(argv[0], *env);
    if (!exec_path)
    {
        print_error(argv[0], NULL, "command not found");
        return (127);
    }
    pid = fork();
    if (pid < 0)
    {
        free(exec_path);
        print_error(argv[0], NULL, strerror(errno));
        return (1);
    }
    if (pid == 0)
    {
        envp = env_to_array(*env);
        if (!envp)
            _exit(1);
        execve(exec_path, argv, envp);
        perror("minishell");
        _exit(126);
    }
    free(exec_path);
    if (waitpid(pid, &status, 0) < 0)
        return (1);
    if (WIFEXITED(status))
        return (WEXITSTATUS(status));
    if (WIFSIGNALED(status))
        return (128 + WTERMSIG(status));
    return (1);
}










