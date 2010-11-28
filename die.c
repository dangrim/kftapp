#include <stdio.h>  /* for perror() */
#include <stdlib.h> /* for exit() */
//THIS CODE WAS ADAPTED FROM http://cs.baylor.edu/~donahoo/practical/CSockets/textcode.html DieWithError.c
void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}
