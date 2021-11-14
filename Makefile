INCL = -I inc
DEPS = inc/malloc.h
SRCDIR = src
OBJDIR = obj
SRC = MyMallocFree.c utils.c
OBJ = $(addprefix $(OBJDIR)/, $(SRC:.c=.o))

NAME = libMyMallocFree.so

CC = gcc
CFLAGS = -Wall -Wextra -fPIC
LDFLAGS = -shared

YELLOW = "\033[33m"
GREEN = "\033[0;32m"


all: $(NAME)

$(NAME): $(DEPS) $(OBJ)
	@$(CC) $(LDFLAGS) $(INCL) -o $(abspath $(NAME)) $(OBJ)
	@echo ${GREEN}$(NAME) has compiled successfully!${NOCOLOR}

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS) $(LIBFT)
	@echo ${YELLOW}Compiling $<${NOCOLOR}
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) $(INCL) -c -o $@ $<

clean:
	@echo ${YELLOW}Cleaning $(OBJDIR)/*.o ${NOCOLOR}
	@rm -rf $(OBJDIR)

fclean: clean
	@echo ${YELLOW}Cleaning *.so $(NAME) ${NOCOLOR}
	@rm -f *.so
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re 

