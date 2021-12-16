CC = g++
I_FLAG = -I Include/
DEBUG_FLAGS = -fsanitize=address,leak,undefined -Wall -g

all: build

build: main.o Tree.o LogsLib.o TextLib.o
	$(CC) main.o Tree.o LogsLib.o TextLib.o -o lang 
debug: main.o Tree.o LogsLib.o TextLib.o 
	$(CC) main.o Tree.o LogsLib.o TextLib.o -o lang $(DEBUG_FLAGS)
suff: main.o LogsLib.o  Language.o Suff_tree.o 
	$(CC) main.o LogsLib.o Language.o Suff_tree.o -o suff $(DEBUG_FLAGS)

clear:
	rm -rf *.o


main.o: main.cpp
	$(CC) -c main.cpp $(I_FLAG)
Tree.o: Tree/Tree.cpp
	$(CC) -c Tree/Tree.cpp $(I_FLAG)
LogsLib.o: Logger/LogsLib.cpp
	$(CC) -c Logger/LogsLib.cpp $(I_FLAG)
TextLib.o: TextLib/TextLib.cpp
	$(CC) -c TextLib/TextLib.cpp $(I_FLAG)
Suff_tree.o: Suff_tree/Suff_tree.cpp
	$(CC) -c Suff_tree/Suff_tree.cpp $(I_FLAG)
Language.o:	Language.cpp
	$(CC) -c Language.cpp $(I_FLAG)
