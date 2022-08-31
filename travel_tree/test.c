#include "../minishell.h"

char	**make_command(t_dlist *curr);

void	puterr_exit(char *str)
{
	ft_putstr_fd("minishell: ", 2);
	perror(str);
	exit(errno);
}

int	ft_free(char **split)
{
	int	i;

	i = 0;
	while (split[i])
	{
		free(split[i]);
		i++;
	}
	free(split);
	return (1);
}

int	execute_line(t_info *info, t_tree *myself)
{
	t_ftool	tool; // pipefd[2], status, pid

	if (pipe(tool.p_fd) == -1)
		return (1);
	tool.pid = fork();
	if (!tool.pid)
		execute(info, myself->left_child);
	waitpid(tool.pid, &tool.status, 0);
	if (myself->dlist->token[0] == '&' && !WEXITSTATUS(tool.status))
		execute(info, myself->right_child);
	else if (myself->dlist->token[0] == '|' && WEXITSTATUS(tool.status))
		execute(info, myself->right_child);
}

int	execute_pipe(t_info *info, t_tree *myself)
{
	int	stdin_fd;
	int	stdout_fd;
	int	left_check;
	int	right_check;


	printf("execute_pipe : %s\n", myself->dlist->token);
	if (!info->pipe_cnt)
	{
		if (pipe(info->pipe) == -1)
			return (-1);
		stdout_fd = dup(STDOUT_FILENO);
		stdin_fd = dup(STDIN_FILENO);
		dup2(info->pipe[1], STDOUT_FILENO);
		// dup2(info->pipe[0], STDIN_FILENO);
	}
	info->pipe_cnt++;
	left_check = execute(info, myself->left_child);
	if (left_check)
		return (left_check);
	dup2(info->pipe[0], STDIN_FILENO);
	// dup2(stdout_fd, STDOUT_FILENO);
	// dup2(stdout_fd, STDOUT_FILENO);
	info->pipe_cnt--;
	if (!info->pipe_cnt)
	{
		close(info->pipe[1]);
		write(2, "HEY!!\n", 6);
		dup2(stdout_fd, STDOUT_FILENO);
		// dup2(stdin_fd, STDIN_FILENO);
	}
	right_check = execute(info, myself->right_child);
	// dup2(info->pipe[1], STDOUT_FILENO);
	// dup2(stdin_fd, STDIN_FILENO);
	return (right_check);
}

int	arg_num(t_dlist *list)
{
	t_dlist *curr;
	int		i;

	curr = list;
	while (list)
	{
		i++;
		list = list->next;
	}
	return (i);
}

char	**make_command_redir(t_tree *left_c, t_tree *right_c)
{
	t_dlist	*left;
	t_dlist *right;

	left = left_c->dlist;
	right = right_c->dlist;
	left->next = right;
	return (make_command(left));
}

int	redir_input(t_info *info, t_tree *myself)
{
	int		r_fd;
	char	**arg;

	if (myself->left_child)
	{
		if (arg_num(myself->right_child->dlist) == 1)
		{
			r_fd = open(myself->right_child->dlist->token, O_RDONLY);
			if (r_fd == -1)
				return (0);
		}
		else if (arg_num(myself->right_child->dlist) >= 2)
		{
			myself->right_child->dlist = myself->right_child->dlist->next;
			// if (!is_builtin(myself->left_child))
				arg = make_command_redir(myself->left_child, myself->right_child);
			info->redir_in_flag = 1;
		}
		if (!info->redir_in_flag) // first redir
		{
			dup2(r_fd, STDIN_FILENO);
			close(r_fd);
			info->redir_in_flag = 1;
		}
		return (execute(info, myself->left_child));
	}
	else
	{
		if (myself->right_child)
			r_fd = open(myself->right_child->dlist->token, O_RDONLY);
		if (!info->redir_in_flag) // first redir
		{
			dup2(r_fd, STDIN_FILENO);
			close(r_fd);
			info->redir_in_flag = 1;
			return (execute(info, myself->left_child)); // execute에서 REDIR가 아니면 flag off 해줘야 함
		}
		else
			return (execute(info, myself->left_child));
	}
}

int	redir_output(t_info *info, t_tree *myself)
{
	int	r_fd;

	if (myself->dlist->token[1] == '>')
		r_fd = open(&myself->dlist->token[2], O_CREAT | O_APPEND | O_RDWR, 0664);
	else
		r_fd = open(&myself->dlist->token[1], O_CREAT | O_TRUNC | O_RDWR, 0664);
	if (r_fd == -1 || (!myself->right_child && !myself->left_child))
		exit(1);
	if (!myself->right_child && !info->redir_out_flag)
	{
		dup2(r_fd, STDOUT_FILENO);
		close(r_fd);
		info->redir_out_flag = 1;
		execute(info, myself->left_child);
	}
	else if (myself->right_child && myself->left_child->dlist->type == REDIR)
	{
		
		// myself->right_child를 myself->left_child->right_child로 보내기
	}
	else if (myself->left_child->dlist->type == WORD);
		// left right 합쳐서 인자로 넘겨주기
		// excute left
}

int	execute_redir(t_info *info, t_tree *myself)
{
	char	*tmp;

	tmp = myself->dlist->token;
	if (tmp[0] == '<' && tmp[1] != '<')
		redir_input(info, myself);
	else if (tmp[0] == '>')
		redir_output(info, myself);
	//if (!ft_strncmp(myself->dlist->token, "<", 2))
	//{
			//}
	//else if (!ft_strncmp(myself->dlist->token, ">", 2))
	//{
	
	//}
	//else if (!ft_strncmp(myself->dlist->token, "<<", 3))
	//{
	
	//}
	//else if (!ft_strncmp(myself->dlist->token, ">>", 3))
	//{
	
	//}
}

char	**make_command(t_dlist *curr)
{
	t_dlist	*tmp;
	char	**ret;
	int		i;

	tmp = curr;
	i = 0;
	while (tmp)
	{
		tmp = tmp->next;
		i++;
	}
	ret = ft_calloc(i + 1, sizeof(char *));
	tmp = curr;
	i = 0;
	while (tmp)
	{
		ret[i] = tmp->token;
		tmp = tmp->next;
		i++;
	}
	return (ret);
}

char	*split_path(char *env, char *cmd)
{
	char	*path;
	char	**splited;
	char	*dir;
	int		i;

	i = 0;
	splited = ft_split(env, ':');
	while (splited[i])
	{
		dir = ft_strjoin(splited[i], "/");
		path = ft_strjoin(dir, cmd);
		free(dir);
		if (access(path, F_OK) > -1)
		{
			ft_free(splited);
			return (path);
		}
		free(path);
		i++;
	}
	ft_free(splited);
	return (cmd);
}

char	*get_path(char *cmd, char **env)
{
	char	*path;
	int		i;

	i = 0;
	while (env[i] && ft_strncmp(env[i], "PATH=", 5))
		i++;
	env[i] += 5;
	path = split_path(env[i], cmd);
	return (path);
}

int	execute_word(t_info *info, t_tree *myself)
{
	char	**argv;
	char	**env;
	char	*path;
	t_ftool	tool;

	myself->dlist = get_first(myself->dlist);
	argv = make_command(myself->dlist);
	env = make_command(info->env);
	path = get_path(argv[0], env);
	tool.pid = fork();
	if (!tool.pid)
		if (execve(path, argv, env) == -1)
			puterr_exit("execve");
	waitpid(tool.pid, &tool.status, 0);
	return (WEXITSTATUS(tool.status));
}

int	execute_bracket(t_info *info, t_tree *myself)
{
	return 1;
}

int	execute(t_info *info, t_tree *myself)
{
	if (myself->dlist->type == LINE)
		return (execute_line(info, myself));
	if (myself->dlist->type == PIPE)
		return (execute_pipe(info, myself));
	if (myself->dlist->type == REDIR)
		return (execute_redir(info, myself));
	if (myself->dlist->type == WORD)
		return (execute_word(info, myself));
	if (myself->dlist->type == BRACKET)
		return (execute_bracket(info, myself));
}
