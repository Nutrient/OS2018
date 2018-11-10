#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <mmu.h>

#define NUMPROCS 4
#define PAGESIZE 4096
#define PHISICALMEMORY 12*PAGESIZE 
#define TOTFRAMES PHISICALMEMORY/PAGESIZE
#define RESIDENTSETSIZE PHISICALMEMORY/(PAGESIZE*NUMPROCS)

extern char *base;
extern int framesbegin;
extern int idproc;
extern int systemframetablesize;
extern int ptlr;

extern struct SYSTEMFRAMETABLE *systemframetable;
extern struct PROCESSPAGETABLE *ptbr;

// funciones que provienen de mmu.c
//---------------
extern int copyframe(int sframe,int dframe);
extern int writeblock(char *buffer, int dblock);
extern int readblock(char *buffer, int sblock);
extern int loadframe(int frame);
extern int saveframe(int frame);
//--------------

int getfreeframe();
int searchvirtualframe();
int getLastAccessFramenumber();

int virtualFrameTableStart;
int virtualFrameTableEnd; 

int pagefault(char *vaddress)
{
    int i;
    int frame,vframe; 
    long pag_a_expulsar;
    int fd;
    char buffer[PAGESIZE];
    int pag_del_proceso;

    virtualFrameTableStart = systemframetablesize + framesbegin; //inicio de memoria secundaria (archivo swap)
    virtualFrameTableEnd = (2 * systemframetablesize) + framesbegin; //fin de memoria secundaria (archivo swap)

    // A partir de la dirección que provocó el fallo, calculamos la página
    pag_del_proceso=(long) vaddress>>12;


    // Si la página del proceso está en un marco virtual del disco
    if((ptbr+pag_del_proceso)->presente != 1 && (ptbr+pag_del_proceso)->framenumber != -1)
    {
	frame = (ptbr+pag_del_proceso)->framenumber;
	// Lee el marco virtual al buffer
	readblock(buffer, frame);
        // Libera el frame virtual
	systemframetable[frame].assigned=0;
	
    }


    // Cuenta los marcos asignados al proceso
    i=countframesassigned();

    // Si ya ocupó todos sus marcos, expulsa una página
    if(i>=RESIDENTSETSIZE)
    {
	// Buscar una página a expulsar
	pag_a_expulsar = getLastAccessFramenumber();	
	// Poner el bitde presente en 0 en la tabla de páginas
        (ptbr+pag_a_expulsar)->presente=0;
        // Si la página ya fue modificada, grábala en disco
	if((ptbr+pag_a_expulsar)->modificado==1)
        {
		// Escribe el frame de la página en el archivo de respaldo y pon en 0 el bit de modificado
		frame = (ptbr+pag_a_expulsar)->framenumber;
		saveframe(frame);
		(ptbr+pag_a_expulsar)->modificado==0;
        }
		
        // Busca un frame virtual en memoria secundaria
	vframe = searchvirtualframe();
	// Si no hay frames virtuales en memoria secundaria regresa error
	if(vframe == -1)
	{
            return(-1);
        }
        // Copia el frame a memoria secundaria, actualiza la tabla de páginas y libera el marco de la memoria principal
	copyframe(frame, vframe); //copiar el frame de memoria secundaria a memoria principal
	(ptbr+pag_a_expulsar)->presente = 0; //indicamos que la pagina se encuentra en memoria secundaria
	(ptbr+pag_a_expulsar)->framenumber = vframe; //cambiamos la pagina al valor de la pagina en memoria virtual
	systemframetable[frame].assigned = 0; //liberamos el frame fisico
	systemframetable[vframe].assigned = 1; //asigmanos el frame virtual
	
    }

    // Busca un marco físico libre en el sistema

    frame=getfreeframe();
    // Si no hay marcos físicos libres en el sistema regresa error
    if(frame==-1 && ptbr[pag_del_proceso].framenumber==-1)
    {
        return(-1); // Regresar indicando error de memoria insuficiente
    }
    else // Si la página estaba en memoria secundaria
    {
        // Cópialo al frame libre encontrado en memoria principal y transfiérelo a la memoria física
	writeblock(buffer, frame);
    }
   
    // Poner el bit de presente en 1 en la tabla de páginas y el frame 
    (ptbr+pag_del_proceso)->presente=1;
    (ptbr+pag_del_proceso)->framenumber=frame;
    return(1); // Regresar todo bien
}



int getfreeframe(){
    int i;
    // Busca un marco libre en el sistema
    for(i=framesbegin;i<systemframetablesize+framesbegin;i++)
        if(!systemframetable[i].assigned)
        {
            systemframetable[i].assigned=1;
            break;
        }
    if(i<systemframetablesize+framesbegin)
        systemframetable[i].assigned=1;
    else
        i=-1;
    return(i);
}
int searchvirtualframe(){
     int i;
    // Busca un marco libre en la memoria virtual sistema
    for(i=virtualFrameTableStart;i<virtualFrameTableEnd;i++)
        if(!systemframetable[i].assigned)
        {
            systemframetable[i].assigned=1;
            break;
        }
    if(i<virtualFrameTableEnd)
        systemframetable[i].assigned=1;
    else
        i=-1;
    return(i);
}

int getLastAccessFramenumber(){
    int frame, i;
    unsigned long lastAccessTime = 0;

    for(i=0;i<ptlr;i++){
	if((ptbr+i)->presente == 1 && (ptbr+i)->tlastaccess > lastAccessTime){
//printf("i = %i, FRAME = %X | PRESENTE = %i | TLASTACCESS = %lu | LASTACCESS = %lu\n",i,(ptbr+i)->framenumber, (ptbr+i)->presente, (ptbr+i)->tlastaccess, lastAccessTime);
	    frame = i;
	    lastAccessTime = (ptbr+i)->tlastaccess;
	}
    }    
    return frame;
}
