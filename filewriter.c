#include <stdio.h>   /* required for file operations */
#include <string.h>

FILE *file;            /* declare the file pointer */

int main(int argc, char *argv[])

{
	char *string;
	int writeSize;
	int result;

	string = argv[2];
	writeSize = strlen(argv[2]);
	file = fopen (argv[1], "a+");  /* open the file for reading */

   /* "rb" means open the file for reading bytes */

	fwrite(string, 1, writeSize, file);
	fclose(file);  /* close the file prior to exiting the routine */
} /*of main*/
