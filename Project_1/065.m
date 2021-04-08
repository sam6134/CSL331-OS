all:
	gcc -o 065.Task1.out 065.Task1.c
	gcc -o 065.Task2.out 065.Task2.c -pthread
clean:
	rm 065.Task1.out 065.Task2.out