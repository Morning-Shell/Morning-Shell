#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <stdlib.h>
# include <signal.h>
# include <termios.h>
# include <fcntl.h>
# include "libft/libft.h"
# include <errno.h>
# include <dirent.h>
# include <sys/wait.h>

/* yehyun
command
option
operator(논리 연산자)
redirection
pipe
single quote
double quote
string -> 달러도 여기서 받아서 예외처리

현재 알려진 이슈 : '$'와 "$"를 어떻게 구분할 것인가??
*/ 

# define LINE 1
# define PIPE 2
# define REDIR 3
# define WORD 4
# define BRACKET 5

# define FALSE 0
# define TRUE 1

typedef struct s_operation
{
	int	i;
	int	last_idx;
}		t_operation;

typedef struct s_ftool
{
	int		p_fd[2];
	int		status;
	pid_t	pid;
}			t_ftool;

typedef struct s_dlist
{
	struct s_dlist	*next;
	struct s_dlist	*prev;
	char			*token;
	int				type;
}					t_dlist;

typedef struct s_tree
{
	t_dlist				*dlist;
	struct s_tree		*right_child;
	struct s_tree		*left_child;
}						t_tree;

typedef struct s_info
{
	char					quote;
	int						quote_flag;
	int						double_quote_flag;
	int						redir_out_flag; // 1이면 > 한개, 2면 >>
	int						redir_in_flag;
	int						s_flag;
	int						*pipe;
	int						pipe_cnt;
	char					**envp;
	struct s_dlist			*dlist;
	t_tree					*root;
	struct s_dlist			*env;
	struct s_operation		fo;
}							t_info;
/* signal.c */
void	set_signal_handler(void);
void	signal_handler(int signal);
void	set_terminal(void);

/*---minishell/parser---*/

	/* lexer.c */
int		lexer(char *str, t_info *info);

	/* input_check.c */
int		input_check(char *str);
int		space_check(char *str);

	/* tokenize.c */	
void	tokenize(t_info *info);
void	cut_node(t_dlist *curr, int i);

	/* syntax.c, syntax_table.c */
int		put_syntaxerr_msg(char *token);
int		check_syntax(t_info *info);
int		check_redir(t_dlist *curr);
int		check_pipe(t_dlist *curr);
int		check_word(t_dlist *curr);
int		check_line(t_dlist *curr);
int		check_bracket(t_dlist *curr);

/*---minishell/utils---*/


/* execute.c */
int	execute(t_info *info, t_tree *myself);

	/* doubly_list.c */
t_dlist	*create_list(void);
void	add_list(t_info *info, char *str);
void	add_list_env(t_info *info, char *str);
int		delete_node(t_dlist **list, t_dlist *node);
void	delete_dlist(t_info *info);
void	printList(t_info *info);

	/* tree.c */
t_tree *make_tree(t_tree *myself, t_dlist *dlist);
void	printTree(t_tree *parent, int cnt);
t_dlist	*get_first(t_dlist *curr);

	/* other */
char	*ft_strjoin_free(char *s1, char *s2);
char	*ft_strrep(char *token, char *value, int i);
/*---minishell/utils---*/

	/* expand.c */
int		expand(t_tree *myself, t_info *info);
int		shell_var_expand(t_dlist *curr, t_info *info);

	/* wildcard.c */
int		wildcard(t_dlist **now);
int		set_list(t_dlist *curr, t_dlist *new_list);
char	*ft_str_rep_wildcard(char *d_name, char *next_path);
char	*ft_strjoin_free(char *s1, char *s2);
int		filter_wildcard(char *wc, char *str, int i, int j);

#endif