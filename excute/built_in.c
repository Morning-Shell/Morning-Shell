#include "../minishell.h"

int	is_echo_option(char *arg)
{
	int	i;

	if (arg[0] != '-')
		return (0);
	i = 1;
	while (arg[i])
	{
		if (arg[i] != 'n')
			return (0);
		i++;
	}
	return (1);
}

int	echo(t_dlist *list)
{
	int		i;
	int		print_i;
	int		opt;
	char	**argv;

	i = 0;
	opt = 0;
	argv = make_command(list->next);
	while (argv[i] && is_echo_option(argv[i]))
		i++;
	if (i > 0)
		opt = 1;
	print_i = i;
	while (argv[i])
	{
		if (i == print_i)
			printf("%s", argv[i]);
		else
			printf(" %s", argv[i]);
		i++;
	}
	if (!opt)
		printf("\n");
	free(argv);
	return (0);
}

int	unset(t_info *info, t_dlist *list)
{
	t_dlist	*tmp;
	char	**key;
	int		i;

	key = make_command(list->next);
	i = 0;
	while (key[i])
	{
		tmp = info->env;
		if ((ft_isdigit(key[i][0]) || key[i][0] == '-')
			&& printf("morningshell: unset: `%c': not a valid identifier\n", key[i][0]) && ++i)
			continue ;
		while (tmp)
		{
			if (ft_strncmp(tmp->token, key[i], ft_strlen(key[i]) + 1) == '=')
				break ;
			tmp = tmp->next;
		}
		if (!tmp && ++i)
			continue ;
		delete_node(&info->env, tmp);
		i++;
	}
	free(key);
	return (0);
}

int	env(t_info *info, int flag)
{
	t_dlist	*temp;

	temp = info->env;
	while (temp != NULL)
	{
		if (flag == 1)
			printf("declare -x ");
		printf("%s\n", temp->token);
		temp = temp->next;
	}
	return (0);
}

int	env_check(t_info *info, t_dlist *env_list, char *key_value, int *i)
{
	char	*temp;
	t_dlist *unset_list;

	while (env_list)
	{
		if (!ft_strncmp(key_value, env_list->token, (*i)))
		{
			temp = ft_strndup(env_list->token, (*i));
			unset_list = create_list();
			unset_list->next = create_list();
			unset_list->next->token = temp;
			unset(info, unset_list);
			free(temp);
			free(unset_list->next);
			free(unset_list);
			break ;
		}
		env_list = env_list->next;
	}
	return (0);
}

int key_check(char *token, int *i)
{
	while (token[(*i)] && token[(*i)] != '=')
	{
		if (ft_isdigit(token[0])) //export 1=e
		{
			printf("minishell: export: `%s': not a valid identifier\n", token);
			return (1);
		}
		if (!token[(*i)]) // export a 
			return (1);
		(*i)++;
	}
	return (0);
}

int	export(t_info *info, t_dlist *list)
{
	int		i;
	t_dlist	*curr;
	t_dlist	*env_list;

	curr = list->next;
	env_list = info->env;
	if (!curr)
	{
		env(info, 1);
		return (0);
	}
	while (curr)
	{
		i = 0;
		if (!key_check(curr->token, &i) && !env_check(info, env_list, curr->token, &i))
			add_list(&info->env, curr->token);
		curr = curr->next;
	}
	return (0);
}

char	*get_home(t_info *info) // HOME을 찾기. HOME을 떄 cd하면  이거 뜸 
{
	t_dlist	*curr;
	char	*ret;
	
	curr = info->env;
	while (curr && ft_strncmp(curr->token, "HOME=", 5))
		curr = curr->next;
	if (!curr)
		return (NULL);
	ret = curr->token;
	ret += 5;
	return (ret);
}

int	cd(t_info *info, t_dlist *list) // 그냥 cd 했을때 처리
{
	static char	*old_path;
	char		*tmp;
	char		*env;
	t_dlist		*env_list;
	t_dlist		*prev_lst;

	if (!list->next) // env에서 $HOME 찾아서 넣어주면 됨
	{
		if (!get_home(info))
			return (put_str_err(list, "HOME not set")); //[bash: cd: HOME not set]
		else
			chdir(get_home(info));
		return (0);
	}
	if (list->next->next)
		return (put_str_err(list, "too many arguments"));
	if (list->next->token[0] == '-' && list->next->token[1] == '\0')
	{
		if (!old_path)
			return (put_str_err(list, "OLDPWD not set"));
		else
		{
			tmp = getcwd(NULL, 0);
			chdir(old_path);
			old_path = tmp;
			tmp = getcwd(NULL, 0);
			printf("%s\n", tmp);
			free(tmp);
		}
	}
	else
	{
		old_path = getcwd(NULL, 0);
		if (chdir(list->next->token))
			puterr_exit_code("cd", 0, 0);
	}
	env = ft_strjoin("OLDPWD=", old_path);
	prev_lst = create_list();
	env_list = create_list();
	env_list->token = env;
	prev_lst->next = env_list;
	export(info, prev_lst);
	free(env);
	free(prev_lst);
	free(env_list);
	free(old_path);
	return (0);
}

int	pwd(void) // 뒤에 오는 argv들 모두 금이빨 bogo빼 씹어먹음
{
	char	*tmp;

	tmp = getcwd(NULL, 0);
	printf("%s\n", tmp);
	free(tmp);
	return (0);
}

int	mini_exit(t_dlist *list)
{
	unsigned char	exit_code;

	if (list->next && !ft_isdigit_str(list->next->token))
	{
		// exit
		// bash: exit: www: numeric argument required
		ft_putstr_fd("exit\nmorningshell: exit: ", 2);
		ft_putstr_fd(list->next->token, 2);
		ft_putendl_fd(": numeric argument required", 2);
		exit(255);
	}
	if (list->next && list->next->next) //안끝남
		return (put_str_err(list, "too many arguments"));
	if (list->next)
		exit_code = ft_atoi(list->next->token);
	else
		exit_code = 0;
	printf("exit\n");
	exit(exit_code);
}

int	built_in(t_info *info, t_tree *myself)
{
	char	*token;

	token = myself->dlist->token;
	if (!ft_strncmp(token, "echo", 5))
		return (echo(myself->dlist));
	else if (!ft_strncmp(token, "cd", 3))
		return (cd(info, myself->dlist));
	else if (!ft_strncmp(token, "unset", 6))
		return (unset(info, myself->dlist));
	else if (!ft_strncmp(token, "env", 4))
		return (env(info, 0));
	else if (!ft_strncmp(token, "pwd", 4))
		return (pwd());
	else if (!ft_strncmp(token, "export", 7))
		return (export(info, myself->dlist));
	else if (!ft_strncmp(token, "exit", 5))
		return (mini_exit(myself->dlist));
	return (-1);
}
