all: 065.Task1-a.c 065.Task1-b.c 065.Task2.c
	gcc 065.Task1-a.c -o 065.Task1-a.out -pthread
	gcc 065.Task1-b.c -o 065.Task1-b.out -pthread
	gcc 065.Task2.c -o 065.Task2.out -pthread