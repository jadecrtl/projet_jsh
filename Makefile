# Directories
OBJ_DIR = obj

# Source files
SRCS =  src/free_all.c \
		src/main.c \
		src/my_exit.c \
		src/my_pwd.c \
		src/signal.c \
		src/split.c \
		src/cd.c \
		src/cd_utils.c \
		src/prompt.c \
		src/redirections.c \
		src/jobs.c \
		src/my_kill.c \
		src/my_bg.c \
		src/my_fg.c \
		src/substitution.c \

OBJS = $(addprefix $(OBJ_DIR)/,$(SRCS:.c=.o)) \
# Program name
NAME = jsh

# Compiler and flags
CC = cc
CFLAGS = -Wall -Wextra -Werror -I./includes -pedantic -g3 -O3
VFLAGS = -s --track-fds=yes --track-origins=yes --leak-check=full --show-leak-kinds=all

# Rule to compile .c files to .o files and move them to the obj directory
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Remove command
RM = rm -rf

# Build the program
$(NAME): $(OBJS)
	$(CC) -o $(NAME) $(OBJS) -lreadline

all: $(NAME)

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

leaks:
	make && clear && valgrind --suppressions=.ignore_leaks.txt $(VFLAGS) ./jsh

re: fclean all

.PHONY: all clean fclean re leaks
