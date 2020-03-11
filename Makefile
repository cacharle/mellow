CC = gcc
LIB = ar rcs
RM = rm -f

HOSTTYPE ?= $(shell uname -m)_$(shell uname -s)
NAME = libft_malloc_$(HOSTTYPE).so

SRCDIR = src
OBJDIR = obj
INCDIR = include

CCFLAGS = -I$(INCDIR) -Wall -Wextra #-Werror

INCLUDE = $(shell find $(INCDIR) -type f -name '*.h')
SRC = $(shell find $(SRCDIR) -type f -name '*.c')
OBJ = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

all: prebuild $(NAME)

prebuild:
	@mkdir $(OBJDIR)

$(NAME): $(OBJ)
	$(LIB) $(NAME) $(OBJ)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(INCLUDE)
	$(CC) $(CCFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all
