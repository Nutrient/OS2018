#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "../include/filesapi.h"

char buffer[1024];

int main()
{
	int i,n;

	int fd;

	int x;

	char buffer[512];

	x=vdopen("pensamiento.txt",0);	// En el sistema de archivos de UNIX

	
	do
	{
		n=vdread(x,buffer,512);	// Leer el archivo del disco virtual
		write(1,buffer,n);	// Escribirlo en pantalla
	} while(n==512);

	close(x);
}


