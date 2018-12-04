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

	int x,y;

	char buffer[512];

	x=open("pensamiento.txt",0);	// En el sistema de archivos de UNIX
	y=vdcreat("pensamiento.txt",0644);	// En el disco virtual
	printf("Archivo creado\n");

	
	do
	{
		n=read(x,buffer,512);	// Leer el archivo de UNIX
		vdwrite(y,buffer,n);	// Escribirlo en el disco virtual
	} while(n==512);

	close(x);
	vdclose(y);

}


