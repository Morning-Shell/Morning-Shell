#include "../minishell.h"

int	execute_line(t_info *info, t_tree *myself)
{
	int	left;
	int	right;

	left = execute(info, myself->left_child);
	if (myself->dlist->token[0] == '&' && !left)
		right = execute(info, myself->right_child);
	else if (myself->dlist->token[0] == '|' && left)
		right = execute(info, myself->right_child);
	else
		return (left);
	return (right);
}

void	pipe_child(t_info *info, t_tree *myself, t_ftool *tool, int flag)
{
	if (flag)
	{
		dup2(tool->p_fd[1], STDOUT_FILENO);
		close(tool->p_fd[1]);
		close(tool->p_fd[0]);
		tool->status = execute(info, myself->left_child);
	}
	else
		tool->status = execute(info, myself->right_child);
	exit(tool->status);
}

void zombie_handler()
{
    int status;
    int spid;
    spid = wait(&status);
}

int	execute_pipe(t_info *info, t_tree *myself)
{
	t_ftool	left_tool;
	t_ftool	right_tool;
	int		tmp_fd;

	signal(SIGCHLD, (void *)zombie_handler);
	tmp_fd = dup(STDIN_FILENO);
	pipe(left_tool.p_fd);
	left_tool.pid = fork();
	if (!left_tool.pid)
		pipe_child(info, myself, &left_tool, 1l);
	waitpid(left_tool.pid, &left_tool.status, WNOHANG);
	dup2(left_tool.p_fd[0], STDIN_FILENO);
	close(left_tool.p_fd[0]);
	close(left_tool.p_fd[1]);
	right_tool.pid = fork();
	if (!right_tool.pid)
		pipe_child(info, myself, &right_tool, 0);
	waitpid(right_tool.pid, &right_tool.status, 0);
	kill(left_tool.pid, SIGKILL);
	dup2(tmp_fd, STDIN_FILENO);
	close(tmp_fd);
	return (WEXITSTATUS(right_tool.status));
}

char	*fix_bracket(char *token)
{
	char	*fixed;
	char	tmp;
	int		len;

	fixed = ft_strdup(token);
	len = ft_strlen(fixed);
	ft_strlcpy(&fixed[0], &fixed[1], len);
	len -= 2;
	tmp = fixed[len - 1];
	ft_strlcpy(&fixed[len - 1], &fixed[len], 2);
	len--;
	fixed[len] = tmp;
	return (fixed);
}

int	execute_bracket(t_info *info, t_tree *myself)
{
	char	*str;
	t_info	infoo;
	t_ftool	tool;

	str = fix_bracket(myself->dlist->token);
	init_info(&infoo);
	infoo.env = info->env;
	tool.pid = fork();
	if (!tool.pid)
	{
		if (space_check(str) == TRUE)
		{
			free(str);
			exit(0);
		}
		if (!input_check(str))
		{
			free(str);
			exit(0);
		}
		if (!lexer(str, &infoo))
		{
			free(str);
			delete_dlist(infoo.dlist);
			exit(0);
		}
		infoo.root = make_tree(NULL, infoo.dlist);
		expand(&infoo, infoo.root);
		tool.status = execute(&infoo, infoo.root);
		free(str);
		free_tree(infoo.root);
		exit(tool.status);
	}
	waitpid(tool.pid, &tool.status, 0);
	free(str);
	return (0);
}

int	execute(t_info *info, t_tree *myself)
{
	if (!myself)
		return (0);
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
	return (0);
}
