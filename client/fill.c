#include <stdio.h>
#include "../packet.h"

int main()
{
	FILE * stream;
	char filename[9];
	char bytes[NFILES*1000];

	for(size_t i = 0; i < NFILES*1000; i++)
	{
		bytes[i] = '0';
	}

	for(size_t i = 0; i < NFILES; i++)
	{
		sprintf(filename, "%d.txt", i);

		stream = fopen(filename, "w");

		fwrite(bytes, sizeof(char), 1000*(i+1), stream);

		fclose(stream);
	}
	return 0;
}
