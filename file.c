#include <stdio.h>   /* required for file operations */
#include <string.h>
#include "kft.h"

FILE *read_file;

void read(char *filename, char *buffer, u16 read_length);
void write(char *filename, char *data, u16 length);

void write(char *filename, char *data, u16 length)
{
    FILE *file;
		file = fopen (filename, "a+");  /* open the file for reading */
		fwrite(data, 1, length, file);
		fclose(file);  /* close the file prior to exiting the routine */
}

void read(char *filename, char *buffer, u16 read_length)
{
  int result;
	read_file = fopen(filename, "rb");  /* open the file for reading */
   /* "rb" means open the file for reading bytes */

	result = fread(buffer, 1, read_length-1, read_file);
	buffer[result] = '\0';
	printf("Received: %s\n", buffer);
}
/*int main(int argc, char *argv[])

{
	char *string;
	int writeSize;
	int result;

	string = argv[2];
	writeSize = strlen(argv[2]);
	file = fopen (argv[1], "a+");

	fwrite(string, 1, writeSize, file);
	fclose(file);  /* close the file prior to exiting the routine 
}*/
