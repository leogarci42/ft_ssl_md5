CC = cc
NAME = ft_ssl
CFLAGS = -Wall -Wextra -I./includes/ -g3 -Wno-unused-command-line-argument -fPIE -march=native -ffast-math -funroll-loops -Wignored-attributes
LDFLAGS = -lreadline
DEBUG_CFLAGS = -g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer
DEBUG_LDFLAGS = -fsanitize=address,undefined
OBJ_DIR = obj
SRC =	./src/main.c \
	./src/helpers/helpers.c \
	./src/commands/md5.c \
	./src/commands/sha256.c

OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))
DIRS = $(sort $(dir $(OBJ)))

MAKE = make --no-print-directory

all: $(NAME)

$(NAME): $(OBJ)
	@printf "$(BLUE)Compiling $(NAME)...$(RESET)\n"
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)
	@printf "$(GREEN)Compilation successful!$(RESET)\n"


$(OBJ_DIR)/%.o: %.c | $(DIRS)
	@printf "$(YELLOW)Compiling $<...$(RESET)\n"
	@$(CC) $(CFLAGS) -c $< -o $@

$(DIRS):
	@mkdir -p $@

clean:
	@printf "$(RED)Cleaning object files...$(RESET)\n"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@printf "$(RED)Cleaning executable $(NAME)...$(RESET)\n"
	@rm -f $(NAME)

re: fclean
	@$(MAKE) all 

#test:
#	@./script/test_integration.sh
#	@rm -f X
#	@make --no-print-directory fclean

.PHONY: all clean fclean re $(DIRS) rc 
rc: fclean
	@$(MAKE) re 

.PHONY: debug
debug:
	@$(MAKE) CFLAGS="$(CFLAGS) $(DEBUG_CFLAGS)" LDFLAGS="$(LDFLAGS) $(DEBUG_LDFLAGS)" all


RESET  = \e[0m
RED    = \e[31m
GREEN  = \e[32m
YELLOW = \e[33m
BLUE   = \e[34m
BOLD   = \e[1m
