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
	src/job_logic/teams_function_2.c \
	src/job_logic/messages_function.c \
	src/job_logic/channel_function.c \
	src/job_logic/thread_fucntion.c \
	src/infrastructure/utils.c \
	src/job_logic/user_function.c \
	$(wildcard src/job_logic/job_command*.c) \
	$(wildcard src/infrastructure/command/*.c) \
	$(wildcard src/infrastructure/socket/*.c) \

OBJC := $(SRCC:.c=.o)

CLIENT_SRC = client/src/main.c \
	client/src/connection.c \
	client/src/response.c \
	client/src/help.c \
	client/src/utils.c
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)

CFLAGS = -g3 -Wall -Wextra -Werror

CPPFLAGS = -I include/
LINKED_LIST_DIR = linked_list
LINKED_LIST_LIB = $(LINKED_LIST_DIR)/linked_list.a
CPPFLAGS += -I $(LINKED_LIST_DIR)/include

CLIENT_CPPFLAGS = -I client/include -I include/

NAME = myteams_server
CLIENT_NAME = myteams_cli

MYTEAMS_LIB_DIR = libs/myteams
MYTEAMS_LIB_FLAGS = -L $(MYTEAMS_LIB_DIR) -lmyteams \
	-Wl,-rpath,$(MYTEAMS_LIB_DIR)

all: $(NAME) $(CLIENT_NAME)

$(NAME): $(LINKED_LIST_LIB) $(OBJC)
	$(CC) -o $(NAME) $(OBJC) $(LINKED_LIST_LIB) $(CFLAGS) \
		-I $(MYTEAMS_LIB_DIR)

$(CLIENT_NAME): $(CLIENT_OBJ)
	$(CC) -o $(CLIENT_NAME) $(CLIENT_OBJ) $(CFLAGS) \
		$(MYTEAMS_LIB_FLAGS)

$(LINKED_LIST_LIB):
	$(MAKE) -C $(LINKED_LIST_DIR)

client/src/%.o: client/src/%.c
	$(CC) $(CFLAGS) $(CLIENT_CPPFLAGS) -c $< -o $@

clean:
	$(MAKE) -C $(LINKED_LIST_DIR) clean
	rm -f $(OBJC)
	rm -f $(CLIENT_OBJ)
	find src -type f -name "*.o" -delete
	find client -type f -name "*.o" -delete

fclean: clean
	$(MAKE) -C $(LINKED_LIST_DIR) fclean
	rm -f $(NAME)
	rm -f $(CLIENT_NAME)

re: fclean all
.PHONY: all clean fclean re

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@ -I $(MYTEAMS_LIB_DIR)
