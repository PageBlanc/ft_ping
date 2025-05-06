NAME 	= ft_ping

CC		= cc

CFLAGS	= -Wall -Wextra -Werror -g

SRC 	= srcs/main.c srcs/check_ip.c srcs/utils.c srcs/signal.c srcs/stats.c

OBJ_DIR = objs

OBJ 	= $(SRC:srcs/%.c=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) -lm

$(OBJ_DIR)/%.o: srcs/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re