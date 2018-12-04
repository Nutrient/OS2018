#include <bootsector.h>
#include <seclog.h>
#include <bitmaps.h>

struct SECBOOT sb;
int sb_in_mem=0;

int inodesmap_en_memoria=0;
unsigned char inodesmap[512];

int blocksmap_en_memoria=0;
unsigned char blocksmap[512*7];



// *************************************************************************
// Para el mapa de bits del área de nodos i
// *************************************************************************

int isinodefree(int inode)
{
	int offset=inode/8;
	int shift=inode%8;
	int result;
	int sec_mapa_bits_nodos_i;

	// Checar si el sector de boot está en memoria
	if(!sb_in_mem)
	{
		// Si no está en memoria, cárgalo
		result=vdreadseclog(0,0,(char *) &sb);
		sb_in_mem=1;
	}
	sec_mapa_bits_nodos_i= sb.sec_res; 	//Usamos la información del sector de boot para 
						//determinar en que sector inicia el 
						// mapa de bits de nodos i 
					
	// Ese mapa está en memoria
	if(!inodesmap_en_memoria)
	{
		// Si no está en memoria, hay que leerlo del disco
		result=vdreadseclog(0,sec_mapa_bits_nodos_i,inodesmap);
		inodesmap_en_memoria=1;
	}


	if(inodesmap[offset] & (1<<shift))
		return(0);
	else
		return(1);
}	

int nextfreeinode()
{
	int i,j;
	int result;
	int mapa_bits_nodos_i;
	int sec_mapa_bits_nodos_i;

	// Checar si el sector de boot está en memoria
	if(!sb_in_mem)
	{
		// Si no está en memoria, cárgalo
		result=vdreadseclog(0,0,(char *) &sb);
		sb_in_mem=1;
	}
	sec_mapa_bits_nodos_i= sb.sec_res; 	//Usamos la información del sector de boot para 
						//determinar en que sector inicia el 
						// mapa de bits de nodos i 


	if(!inodesmap_en_memoria)
	{
		result=vdreadseclog(0,sec_mapa_bits_nodos_i,inodesmap);
		inodesmap_en_memoria=1;
	}

	// Recorrer byte por byte mientras sea 0xFF sigo recorriendo
	i=0;
	while(inodesmap[i]==0xFF && i<sb.sec_mapa_bits_nodos_i*512)
		i++;

	if(i<sb.sec_mapa_bits_nodos_i*512)
	{
		j=0;
		while(inodesmap[i] & (1<<j) && j<8)
			j++;

		return(i*8+j);
	}
	else
		return(-1);

		
}

int assigninode(int inode)
{
	int offset=inode/8;
	int shift=inode%8;
	int result;
	int mapa_bits_nodos_i;

	if(!sb_in_mem)
	{
		// Si no está en memoria, cárgalo
		result=vdreadseclog(0,0,(char *) &sb);
		sb_in_mem=1;
	}

	mapa_bits_nodos_i= sb.sec_res;

	if(!inodesmap_en_memoria)
	{
		result=vdreadseclog(0,mapa_bits_nodos_i,inodesmap);
		inodesmap_en_memoria=1;
	}

	inodesmap[offset]|=(1<<shift);
	vdwriteseclog(0,mapa_bits_nodos_i,inodesmap);
	return(1);
}

int unassigninode(int inode)
{
	int offset=inode/8;
	int shift=inode%8;
	int result;
	char mask;
	int mapa_bits_nodos_i; 

	if(!sb_in_mem)
	{
		// Si no está en memoria, cárgalo
		result=vdreadseclog(0,0,(char *) &sb);
		sb_in_mem=1;
	}

	mapa_bits_nodos_i= sb.sec_res;

	if(!inodesmap_en_memoria)
	{
		result=vdreadseclog(0,mapa_bits_nodos_i,inodesmap);
		inodesmap_en_memoria=1;
	}


	inodesmap[offset]&=(char) ~(1<<shift);
	vdwriteseclog(0,mapa_bits_nodos_i,inodesmap);
	return(1);
}	

// ******************************************************************************
// Para el mapa de bits del área de de datos
// ******************************************************************************

int isblockfree(int block)
{
	int offset=block/8;
	int shift=block%8;
	int result;
	int i;
	int mapa_bits_bloques;

	// Es necesario tener el sector de boot en memoria
	// Porque con los parámetros que están ahí
	// Puedo determinar en qué sector está el mapa de bits
	if(!sb_in_mem)
	{
		// Si no está en memoria, cárgalo
		result=vdreadseclog(0,0,(char *) &sb);
		sb_in_mem=1;
	}

	// Calcular en qué sector lógico inicia el mapa de bits
	mapa_bits_bloques= sb.sec_res+sb.sec_mapa_bits_nodos_i;

	// Si ese mapa de bits no está en memoria, cargarlo a memoria
	if(!blocksmap_en_memoria)
	{
		for(i=0;i<sb.sec_mapa_bits_bloques;i++)
			result=vdreadseclog(0,mapa_bits_bloques+i,blocksmap+i*512);
		blocksmap_en_memoria=1;
	}

	if(blocksmap[offset] & (1<<shift))
		return(0);	// Bloque no disponible
	else
		return(1);	// Bloque disponible
}	

int nextfreeblock()
{
	int i,j;
	int result;
	int mapa_bits_bloques;

	if(!sb_in_mem)
	{
		// Si no está en memoria, cárgalo
		result=vdreadseclog(0,0,(char *) &sb);
		sb_in_mem=1;
	}

	mapa_bits_bloques = sb.sec_res+sb.sec_mapa_bits_nodos_i;

	if(!blocksmap_en_memoria)
	{
		for(i=0;i<sb.sec_mapa_bits_bloques;i++)
			result=vdreadseclog(0,mapa_bits_bloques+i,blocksmap+i*512);
		blocksmap_en_memoria=1;
	} 

	i=0;
	while(blocksmap[i]==0xFF && i<sb.sec_mapa_bits_bloques*512)
		i++;

	if(i<sb.sec_mapa_bits_bloques*512)
	{
		j=0;
		while(blocksmap[i] & (1<<j) && j<8)
			j++;

		return(i*8+j);	// El número de bloque que encontré disponible
	}
	else
		return(-1);

		
}

int assignblock(int block)
{
	int offset=block/8;
	int shift=block%8;
	int result;
	int i;
	int sector;
	int mapa_bits_bloques;

	if(!sb_in_mem)
	{
		// Si no está en memoria, cárgalo
		result=vdreadseclog(0,0,(char *) &sb);
		sb_in_mem=1;
	}

	mapa_bits_bloques= sb.sec_res+sb.sec_mapa_bits_nodos_i;

	if(!blocksmap_en_memoria)
	{
		for(i=0;i<sb.sec_mapa_bits_bloques;i++)
			result=vdreadseclog(0,mapa_bits_bloques+i,blocksmap+i*512);
		blocksmap_en_memoria=1;
	} 

	blocksmap[offset]|=(1<<shift);

	sector=(offset/512)*512;
	vdwriteseclog(0,mapa_bits_bloques+sector,blocksmap+sector*512);
	//for(i=0;i<sb.sec_mapa_bits_bloques;i++)
	//	vdwriteseclog(mapa_bits_bloques+i,blocksmap+i*512);
	return(1);
}

int unassignblock(int block)
{
	int offset=block/8;
	int shift=block%8;
	int result;
	char mask;
	int sector;
	int i;
	int mapa_bits_bloques;

	if(!sb_in_mem)
	{
		// Si no está en memoria, cárgalo
		result=vdreadseclog(0,0,(char *) &sb);
		sb_in_mem=1;
	}

	mapa_bits_bloques=(int) sb.sec_res + (int) sb.sec_mapa_bits_nodos_i;

	if(!blocksmap_en_memoria)
	{
		for(i=0;i<sb.sec_mapa_bits_bloques;i++)
		 	result=vdreadseclog(0,mapa_bits_bloques+i,blocksmap+i*512);
		blocksmap_en_memoria=1;
	}

	blocksmap[offset]&=(char) ~(1<<shift);

	sector=(offset/512)*512;
	vdwriteseclog(0,mapa_bits_bloques+sector,blocksmap+sector*512);
	// for(i=0;i<sb.sec_mapa_bits_bloques;i++)
	//	vdwriteseclog(mapa_bits_bloques+i,blocksmap+i*512);
	return(1);
}
