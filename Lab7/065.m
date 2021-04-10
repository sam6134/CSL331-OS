all: 065.Task1.c 065.Task2.c
	gcc 065.Task1.c -o 065.Task1 -Wall -pthread
	gcc 065.Task2.c -o 065.Task2 -Wall -pthread
	gcc 065.Task3.c -o 065.Task3 -Wall -pthread
	gcc 065.Task4.c -o 065.Task4 -Wall -pthread