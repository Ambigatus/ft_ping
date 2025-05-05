NAME = ft_ping

CC = gcc
CFLAGS = -Wall -Wextra -Werror
INCLUDES = -I ./includes

# Source files
SRC_DIR = srcs
SRC_FILES = main.c ping.c socket.c packet.c dns.c display.c
SRCS = $(addprefix $(SRC_DIR)/, $(SRC_FILES))

# Object files
OBJ_DIR = objs
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Rules
all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "ft_ping successfully compiled!"

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	@echo "Object files removed!"

fclean: clean
	rm -f $(NAME)
	@echo "Executable removed!"

re: fclean all

.PHONY: all clean fclean re