##
## EPITECH PROJECT, 2026
## delivery
## File description:
## Makefile
##

CC = clang

SRCC = src/infrastructure/bufferiser.c \
	src/infrastructure/loader.c \
	src/infrastructure/logger_functions.c \
	src/main.c \
	src/infrastructure/server_function.c \
	src/job_logic/teams_function.c \
	src/infrastructure/utils.c \
	src/job_logic/user_function.c \
	$(wildcard src/job_logic/job_command*.c) \
	$(wildcard src/infrastructure/command/*.c) \
	$(wildcard src/infrastructure/socket/*.c) \

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
	find src -type f -name "*.o" -delete

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
