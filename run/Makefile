CC 			 = g++
I_FLAG 		 = -I ../Include/
DEBUG_FLAGS  = -fsanitize=address,leak,undefined -Wall -g
ALL_OBJECTS  = main.o Tree.o TextLib.o Recursive_descent.o Suff_tree.o Language.o Lexer.o TreeFile.o Back_end.o Stack.o 
SUFF_OBJECTS = main.o LogsLib.o  Language.o Suff_tree.o
C_FLAG 		 = -c -O3 -g


all: build


build: $(ALL_OBJECTS) liblogger.so.0.1
	$(CC) $(ALL_OBJECTS) liblogger.so.0.1 -Wl,-rpath=`pwd` -o lang 
debug: $(ALL_OBJECTS) liblogger.so.0.1
	$(CC) $(ALL_OBJECTS) liblogger.so.0.1 -Wl,-rpath=`pwd` -o lang $(DEBUG_FLAGS)
suff: $(SUFF_OBJECTS)
	$(CC) $(SUFF_OBJECTS) -o suff $(DEBUG_FLAGS)

clear:
	rm -rf *.o


main.o: ../main.cpp
	$(CC) $(C_FLAG) ../main.cpp 	$(I_FLAG)
Tree.o: ../Tree/Tree.cpp
	$(CC) $(C_FLAG) ../Tree/Tree.cpp $(I_FLAG)
LogsLib.o: ../Logger/LogsLib.cpp
	$(CC) $(C_FLAG) ../Logger/LogsLib.cpp $(I_FLAG)
TextLib.o: ../TextLib/TextLib.cpp
	$(CC) $(C_FLAG) ../TextLib/TextLib.cpp $(I_FLAG)
Suff_tree.o: ../Suff_tree/Suff_tree.cpp
	$(CC) $(C_FLAG) ../Suff_tree/Suff_tree.cpp $(I_FLAG)
Language.o:	../Language.cpp
	$(CC) $(C_FLAG) ../Language.cpp $(I_FLAG)
Recursive_descent.o: ../Recursive_descent/Recursive_descent.cpp
	$(CC) $(C_FLAG) ../Recursive_descent/Recursive_descent.cpp $(I_FLAG)
Lexer.o: ../Lexer/Lexer.cpp
	$(CC) $(C_FLAG) ../Lexer/Lexer.cpp $(I_FLAG)
TreeFile.o: ../TreeFile/TreeFile.cpp
	$(CC) $(C_FLAG) ../TreeFile/TreeFile.cpp $(I_FLAG)
Back_end.o: ../Back_end/Back_end.cpp
	$(CC) $(C_FLAG) ../Back_end/Back_end.cpp $(I_FLAG)
Stack.o: ../Stack/Stack.cpp
	$(CC) $(C_FLAG) ../Stack/Stack.cpp $(I_FLAG)
Trans.o: ../Trans/trans.cpp
	$(CC) $(C_FLAG) ../Trans/trans.cpp $(I_FLAG)