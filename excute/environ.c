#include "../minishell.h"

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