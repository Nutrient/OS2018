#include <bootsector.h>
#include <seclog.h>
#include <blocks.h>


// **********************************************************************************
// Lectura y escritura de bloques
// **********************************************************************************

struct SECBOOT sb;
extern int sb_in_mem;

int writeblock(int block,char *buffer)
{
	int result;
	int i;
	int inicio_area_datos;
		
	if(!sb_in_mem)
	{
		result=vdreadseclog(0,0,(char *) &sb);
		sb_in_mem=1;
	}

	inicio_area_datos=sb.sec_res+sb.sec_mapa_bits_nodos_i +sb.sec_mapa_bits_bloques+sb.sec_tabla_nodos_i;


	for(i=0;i<sb.sec_x_bloque;i++)
		vdwriteseclog(0,inicio_area_datos+(block-1)*sb.sec_x_bloque+i,buffer+512*i);
	return(1);	
}

int readblock(int block,char *buffer)
{
	int result;
	int i;
	int inicio_area_datos;

	if(!sb_in_mem)
	{
		result=vdreadseclog(0,0,(char *) &sb);
		sb_in_mem=1;
	}
	inicio_area_datos=sb.sec_res+sb.sec_mapa_bits_nodos_i +sb.sec_mapa_bits_bloques+sb.sec_tabla_nodos_i;

	for(i=0;i<sb.sec_x_bloque;i++)
		vdreadseclog(0,inicio_area_datos+(block-1)*sb.sec_x_bloque+i,buffer+512*i);
	return(1);	
}
