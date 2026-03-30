##
## EPITECH PROJECT, 2026
## delivery
## File description:
## Makefile
##

CC = clang

SRCC = src/bufferiser.c \
	src/loader.c \
	src/main.c \
	src/server_function.c \
	src/socket.c \
	src/teams_function.c \
	src/utils.c \
	src/user_function.c \
	$(wildcard src/operation/*.c) \
	$(wildcard src/control_socket/*.c) \

OBJC := $(SRCC:.c=.o)

CFLAGS = -g3 -Wall -Wextra -Werror

CPPFLAGS = -I include/
LINKED_LIST_DIR = linked_list
LINKED_LIST_LIB = $(LINKED_LIST_DIR)/linked_list.a
CPPFLAGS += -I $(LINKED_LIST_DIR)/include

NAME = myteams_server

SIMPLE_SCENARIO_SRC = tests/simple_login_logout_client.c
SIMPLE_SCENARIO_BIN = simple_login_logout

all: $(NAME)

$(NAME): $(LINKED_LIST_LIB) $(OBJC)
	$(CC) -o $(NAME) $(OBJC) $(LINKED_LIST_LIB) $(CFLAGS)

$(LINKED_LIST_LIB):
	$(MAKE) -C $(LINKED_LIST_DIR)


clean:
	$(MAKE) -C $(LINKED_LIST_DIR) clean
	rm -f $(OBJC)

fclean: clean
	$(MAKE) -C $(LINKED_LIST_DIR) fclean
	rm -f $(NAME) $(SIMPLE_SCENARIO_BIN)

re: fclean all

$(SIMPLE_SCENARIO_BIN): $(SIMPLE_SCENARIO_SRC)
	$(CC) -o $(SIMPLE_SCENARIO_BIN) $(SIMPLE_SCENARIO_SRC) $(CFLAGS)

scenario-login-logout: $(SIMPLE_SCENARIO_BIN)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS)

.PHONY: all clean fclean re scenario-login-logout
