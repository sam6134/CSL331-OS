all: 065.c 
	gcc -o 065.out 065.c -pthread 

clean:
	rm 065.out