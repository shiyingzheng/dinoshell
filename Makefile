all: shell
	
shell: shell.c
	clang -g -pedantic -std=c99 -o shell shell.c

clean:
