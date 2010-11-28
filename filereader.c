#include <stdio.h>   /* required for file operations */

FILE *file;            /* declare the file pointer */

int main(int argc, char *argv[])

{
	int readSize;
	int result;
	readSize = atoi(argv[2]);
	char buffer[readSize+1];
	file = fopen (argv[1], "rb");  /* open the file for reading */

   /* "rb" means open the file for reading bytes */

	while(result = fread(buffer, 1, readSize, file))
	{
		buffer[result] = '\0';
		printf("Received: %s\n", buffer);
	}

	
   
   fclose(file);  /* close the file prior to exiting the routine */
} /*of main*/
