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

int	execute_pipe(t_info *info, t_tree *myself)
{
	int		i;
	int		in_fd;
	pid_t	left;
	pid_t	right;
	t_ftool	tool;

	i = 0;
	in_fd = dup(STDIN_FILENO); // in 이나 out이 닫혀있으면 process가 종료됨 
	pipe(tool.p_fd);
	while (i < 2) 
	{
		tool.pid = fork();
		if (!tool.pid && !i)
		{
			dup2(tool.p_fd[1], STDOUT_FILENO);
			close(tool.p_fd[1]);
			close(tool.p_fd[0]);
			tool.status = execute(info, myself->left_child);
			exit(tool.status);
		}
		else if (!tool.pid && i == 1)
		{
			tool.status = execute(info, myself->right_child);
			exit(tool.status);
		}
		else if (!i)
		{
			left = tool.pid;
			dup2(tool.p_fd[0], STDIN_FILENO);
			close(tool.p_fd[0]);
			close(tool.p_fd[1]);
		}
		else if (i == 1)
			right = tool.pid;
		i++;
	}
	dup2(in_fd, STDIN_FILENO);
	waitpid(left, &tool.status, 0);
	waitpid(right, &tool.status, 0);
	return (WEXITSTATUS(tool.status));
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
