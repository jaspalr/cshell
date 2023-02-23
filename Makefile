all: cshell

cshell: cshell.c EnvVarLL.h CommandLL.h ArugumentLL.h
	gcc -g -Wall -o cshell cshell.c EnvVarLL.h CommandLL.h  ArugumentLL.h

clean:
	$(RM) cshell

