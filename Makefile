##
## EPITECH PROJECT, 2026
## delivery
## File description:
## Makefile
##

CCX = clang++
CC = clang

SRC = $(wildcard src/*.cpp) \

SRCC = src/bufferiser.c \
	src/main.c \
	src/socket.c \
	src/utils.c \
	src/user_function.c \
	$(wildcard src/operation/*.c) \
	$(wildcard src/data_socket/*.c) \
	$(wildcard src/control_socket/*.c) \

SRCTEST = $(wildcard tests/test_*.c)
SRCC_TEST = $(filter-out src/main.c,$(SRCC))
CLIENT_TEST_SRC = tests/client_test.c


OBJ := $(SRC:.cpp=.o)
OBJC := $(SRCC:.c=.o)
OBJC_TEST := $(SRCC_TEST:.c=.o)

OBJTEST := $(SRCTEST:.c=.o)

CXXFLAGS = -std=c++20 -g3 -Wall -Wextra -Werror
CFLAGS = -g3 -Wall -Wextra -Werror

CPPFLAGS = -I include/
LINKED_LIST_DIR = linked_list
LINKED_LIST_LIB = $(LINKED_LIST_DIR)/linked_list.a
CPPFLAGS += -I $(LINKED_LIST_DIR)/include

NAME = myteams_server

TESTFOLDER = tests

TESTNAME = unit_tests
CLIENT_TEST_NAME = client_test
CLI_TEST_SCRIPT = clii/run_cli_tests.sh

all: $(NAME)

$(NAME): $(LINKED_LIST_LIB) $(OBJ) $(OBJC)
	$(CCX) -o $(NAME) $(OBJ) $(OBJC) $(LINKED_LIST_LIB) $(CXXFLAGS)

$(LINKED_LIST_LIB):
	$(MAKE) -C $(LINKED_LIST_DIR)


clean:
	$(MAKE) -C $(LINKED_LIST_DIR) clean
	rm -f $(OBJ) $(OBJC) $(OBJTEST)

fclean: clean
	$(MAKE) -C $(LINKED_LIST_DIR) fclean
	rm -f $(NAME) $(TESTNAME) $(CLIENT_TEST_NAME)

re: fclean all

test: $(TESTNAME)
	./$(TESTNAME)

test-client: $(CLIENT_TEST_NAME)

test-cli: $(NAME) $(CLIENT_TEST_NAME)
	bash $(CLI_TEST_SCRIPT) ./$(NAME) ./$(CLIENT_TEST_NAME)

$(TESTNAME): $(OBJTEST) $(OBJC_TEST)
	$(CC) -o $(TESTNAME) $(OBJTEST) $(OBJC_TEST) $(CFLAGS) $(CPPFLAGS) \
	-lcriterion

$(CLIENT_TEST_NAME): $(CLIENT_TEST_SRC)
	$(CC) -o $(CLIENT_TEST_NAME) $(CLIENT_TEST_SRC) $(CFLAGS) $(CPPFLAGS)

%.o: %.cpp
	$(CCX) -c -o $@ $< $(CXXFLAGS) $(CPPFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS)

.PHONY: all clean fclean re test test-client test-cli
