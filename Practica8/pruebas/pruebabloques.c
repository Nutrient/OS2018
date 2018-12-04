
#include <string.h>
#include "../include/blocks.h"

char buffer[1024];

int main()
{
	memset(buffer,'*',1024);
	writeblock(10,buffer);
}