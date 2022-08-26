CC = cc
CFLAGS = -Wall -Wextra -Werror
LINKING_FLAGS = -L/opt/homebrew/Cellar/readline/8.1.2/lib -lreadline
COMFILE_FLAGS = -I/opt/homebrew/Cellar/readline/8.1.2/include/
DFLAGS = -g2 -fsanitize=address

RM = rm -f

PARSER_DIR = parser
UTILS_DIR = utils
EXPAND_DIR = expand

SRCS =	minishell.c \
		signal.c \
		$(PARSER_DIR)/lexer.c \
		$(PARSER_DIR)/tokenize.c \
		$(PARSER_DIR)/input_check.c \
		$(PARSER_DIR)/syntax.c \
		$(PARSER_DIR)/syntax_table.c \
		$(UTILS_DIR)/doubly_list.c \
		$(UTILS_DIR)/tree.c \
		$(UTILS_DIR)/ft_strrep.c \
		$(UTILS_DIR)/ft_strjoin_free.c \
		$(EXPAND_DIR)/expand.c \
		$(EXPAND_DIR)/shell_var.c \
		$(EXPAND_DIR)/wild_card.c \

OBJS = $(SRCS:%.c=%.o)

NAME = minishell
SHELL = bash

all : $(NAME)

$(NAME) : $(OBJS)
	@make -C ./libft
	@./make.sh
	@$(CC) $(DFLAGS) $(CFLAGS) $(OBJS) -o $(NAME) $(LINKING_FLAGS) $(COMFILE_FLAGS) ./libft/libft.a

%.o : %.c
	@$(CC) $(DFLAGS) $(COMFILE_FLAGS) -c $< -o $@

clean :
	@make -C ./libft fclean
	@$(RM) $(OBJS)

fclean :
	@make -C ./libft fclean
	@$(RM) $(OBJS) $(NAME)

re :
	@make fclean
	@make all

run :
	@cat ascii_art/sun
	@make re
	@./minishell

#debug :
#	@make -C ./libft
#	@./make.sh
#	@$(CC) -L/usr/local/opt/readline/lib -I/usr/local/opt/readline/include -lreadline ./libft/libft.a $(SRCS) -o $(NAME) -g3 -fsanitize=address
#	@./minishell

.PHONY : all clean fclean re