/*
 * dsh.c
 *
 *  Created on: March 3rd, 2025
 *      Author: maddyS
 */
#include "dsh.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <errno.h>
#include <err.h>
#include <sys/stat.h>
#include <string.h>


// TODO: Your function definitions below (declarations in dsh.h)


char** split(char *str, char *delim) {

    int i = 0; // iterator for "strin(g)dex"
    int numSubstr = 2; // number of substrings (1 as the first string to get split, 1 as the NULL termination character)
    while (!strcmp(str+i, "\0") == 0) { 
        char ch = str[i++];
        if (ch == ' ') {
            numSubstr++;
        }
    }

    char **strArray = (char**) malloc(numSubstr * sizeof(char*)); // malloc string pointers

    for (int i = 0; i < numSubstr-1; i++) {
        strArray[i] = (char*) malloc(MAXBUF * sizeof(char)); 
    }
    
    // loop through and copy each token into the string array
    char* token = strtok(str, " "); // get first token
    for (int i = 0; i < numSubstr-1; i++) {
        strcpy(strArray[i], token); // copy token (will copy null into the last slot)
        token = strtok(NULL, " "); // get next token (will be null on the last iteration)
    }

    strArray[numSubstr-1] = NULL; // put in terminating string

    return strArray;
}

void freeSplitResult(char **strArray) {
    int i = 0;
    
	do {
		free(strArray[i++]);
	} while (strArray[i] != NULL);

	free(strArray);
}
