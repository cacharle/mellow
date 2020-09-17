# ############################################################################ #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: charles <me@cacharle.xyz>                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/09/17 19:54:24 by charles           #+#    #+#              #
#    Updated: 2020/09/17 19:54:31 by charles          ###   ########.fr        #
#                                                                              #
# ############################################################################ #

CC = gcc
LIB = ar rcs
RM = rm -f

HOSTTYPE ?= $(shell uname -m)_$(shell uname -s)
NAME = libft_malloc_$(HOSTTYPE).so

SRCDIR = src
OBJDIR = obj
INCDIR = include

CCFLAGS = -I$(INCDIR) -Wall -Wextra #-Werror

INC = $(shell find $(INCDIR) -type f -name '*.h')
SRC = $(shell find $(SRCDIR) -type f -name '*.c')
OBJ = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

all: prebuild $(NAME)

prebuild:
	@mkdir -vp $(OBJDIR)

$(NAME): $(OBJ)
	$(LIB) $(NAME) $(OBJ)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(INC)
	$(CC) $(CCFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all
