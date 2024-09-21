CC = gcc
FLAGS = -Wall -std=c11

all: my_shell

my_shell: 
	gcc -c my_shell.c
	gcc my_shell.o -o my_shell
	./my_shell

.PHONY: clean
clean_exc:
	rm -f my_shell
clean:
	rm -f *.o 
