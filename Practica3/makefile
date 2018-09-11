all: serial posixThread threadClone forkThread

serial: serial.c
	gcc -o serial serial.c -lm

posixThread: posixThread.c
	gcc -o posixThread posixThread.c -lm -pthread

threadClone: threadClone.c
	gcc -o threadClone threadClone.c -lm

forkThread: forkThread.c
	gcc -o forkThread forkThread.c -lm