/*
 * dsh.c
 *
 * defines helper functions for davidshell
 *
 * Created on: March 3rd, 2025
 * Author: maddyS
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
#include <stdbool.h>

/**
 * splits a string on the given delimiter
 */
char** split(char *str, char *delim) {

    int i = 0; // iterator for "strin(g)dex"
    int numSubstr = 2; // number of substrings (1 as the first string to get split, 1 as the NULL termination character)
    while (!strcmp(str+i, "\0") == 0) { 
        char ch = str[i++];
        if (ch == *delim) {
            numSubstr++;
        }
    }

    char **strArray = (char**) malloc(numSubstr * sizeof(char*)); // malloc string pointers

    for (int i = 0; i < numSubstr-1; i++) {
        strArray[i] = (char*) malloc(MAXBUF * sizeof(char)); 
    }
    
    // loop through and copy each token into the string array
    char* token = strtok(str, delim); // get first token
    for (int i = 0; i < numSubstr-1; i++) {
        strcpy(strArray[i], token); // copy token (will copy null into the last slot)
        token = strtok(NULL, delim); // get next token (will be null on the last iteration)
    }

    strArray[numSubstr-1] = NULL; // put in terminating string

    return strArray;
}

/**
 * frees all memory that is allocated by split() given the return pointer of split()
 */
void freeSplitResult(char **strArray) {
    int i = 0;
    
	do {
		free(strArray[i++]);
	} while (strArray[i] != NULL);

	free(strArray);
}

/**
 * forks a new process and runs the given command
 */
void runCmd(char **splitCmd, int argc) {
    bool skipWaiting = false;
    if (strcmp(splitCmd[argc], "&") == 0) { // is the last argument an "&"?
        skipWaiting = true; // necessary to tell the child that "&" is not an argument for the new command
        free(splitCmd[argc]); // free it so that commands do not try to parse & as an argument
        splitCmd[argc] = NULL; // write NULL as a new termination where & used to be
    }

    int id = fork();

    if (id) { // in parent
        if (!skipWaiting) { // wait or not?
            // run in foreground
            int doneId = wait(NULL); // wait on child to complete if no & entered
            while (doneId != id) { // this loop reaps all previous & commands if they have completed
                doneId = wait(NULL);
            }
        }
    } else { // in child
        // turn child into specified command
        execv(splitCmd[0], splitCmd); // no need to free memory since execv effectively frees it (by overwriting it)
    }
}

/**
 * returns true if the given string has two or more consecutive spaces
 */
bool hasDoubleSpaces(char *str) {
    bool hasDoubleSpaces = false;
    if (str[0] == '\0') {
        return false;
    }

    int i = 1;
    while (str[i] != '\0') {
        if (str[i] == ' ' && str[i-1] == ' ') {
            hasDoubleSpaces = true;
        }
        i++;
    }
    return hasDoubleSpaces;
}

/**
 * returns true if the given string has only ' ', '\n', or '\0' characters.
 * this is used to prevent segfaults in the main loop and reprompt (split
 * function HATES double spaces)
 */
bool isOnlySpaces(char *str) {
    bool isOnlySpaces = true;
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] != ' ' && str[i] != '\0' && str[i] != '\n') {
            isOnlySpaces = false;
        }
        i++;
    }
    return isOnlySpaces;
}

/**
 * removes leading and trailing spaces
 */
void trimSpaces(char *str) {
    int length = strlen(str);
    
    // find last leading space
    int offset = 0;
    while (str[offset] == ' ') {
        offset++;
    }

    // copy over based on offset found above
    for (int i = 0; i<length-1; i++) {
        str[i] = str[i+offset];
    }
    
    // move terminating character
    int i = 0;
    do {
        str[length-i] = '\0';
        i++;
    } while (str[length-i] == ' ' || str[length-i] == '\0');
}

/**
 * removes all instances of multiple consecutive spaces and replaces
 * them with a single space.
 */
void removeDoubleSpaces(char *str) {
    // this fxn breaks if the first char is not a space, which it shouldn't be based on trim spaces :)
    int length = strlen(str);
    while (hasDoubleSpaces(str)) { // one at a time since more than 3 at a time breaks :(
        int i = 0;
        int offset = 0;
        bool spaceFound = false;
        while (str[i] != '\0') {
            if (str[i] == ' ' && spaceFound) {
                offset = 1;
            } else if (str[i] == ' ') {
                spaceFound = true;
            } else {
                spaceFound = false;
            }
            str[i] = str[i+offset]; // copy over based on number of double spaces so far
            i++;
        }
    }

    // move terminating character
    int i = 0;
    do {
        str[length-i] = '\0';
        i++;
    } while (str[length-i] == ' ' || str[length-i] == '\0');
}

/**
 * runs both trimSpaces() and removeSpaces() on given string
 */
void cleanUpString(char *str) {
    trimSpaces(str);
    removeDoubleSpaces(str);
}