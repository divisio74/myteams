##
## EPITECH PROJECT, 2024
## B-MUL-100-LYN-1-1-myradar-pierre.riss
## File description:
## Makefile
##

CC		= 	gcc
OBJ		=	$(subst src/,obj/,$(patsubst %.c,%.o,$(SRC)))
SRC		=	$(wildcard src/*.c)
ODIR	=	obj
OTHER_LIB =
GCCFLAGS = -I include
CFLAGS =
LIB = linked_list.a

all: $(OBJ)

moove:
	mv $(wildcard *.o) $(ODIR)

$(OBJ) : $(SRC)
	$(CC) -c $(SRC) $(GCCFLAGS)
	make moove
	ar rcs $(LIB) $(OBJ)

clean:
	rm -f $(ODIR)/*.o
	rm -f $(NAME).a

fclean:
	make clean
	rm -f $(LIB)

re: fclean all
