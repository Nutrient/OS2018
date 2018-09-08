all: serial posixThread threadClone

serial: serial.c
	gcc -o serial serial.c -lm

posixThread: posixThread.c
	gcc -o posixThread posixThread.c -lm -pthread

threadClone: threadClone.c
	gcc -o threadClone threadClone.c -lm
