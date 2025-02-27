/*
 * main.c
 *
 *  Created on: Mar 17 2017
 *      Author: david
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include "dsh.h"

#define MAX_PROC 1024

int main(int argc, char *argv[]) {

	// DO NOT REMOVE THE BLOCK BELOW (FORK BOMB PREVENTION) //
	struct rlimit limit;
	limit.rlim_cur = MAX_PROC;
	limit.rlim_max = MAX_PROC;
	setrlimit(RLIMIT_NPROC, &limit);
	// DO NOT REMOVE THE BLOCK ABOVE THIS LINE //

	char *cmdline = (char*) malloc(MAXBUF); // stores user input from commmand line

	while (true) {
		printf("dsh> ");
		fgets(cmdline, MAXBUF, stdin);
		for (int i = 0; i<MAXBUF; i++) {
			if (cmdline[i] == '\n') {
				cmdline[i] = '\0';
				break; // shouldn't make any difference, but this makes valgrind happy :)
			}
		}
		
		char** splitCmd = split(cmdline, " ");

		// handle exit
		if (strcmp(splitCmd[0], "exit") == 0) {
			// TODO: make sure you have cleaned all allocated memory

			free(cmdline); // free input command
			freeSplitResult(splitCmd); // free split command pieces and their pointer array

			break; // exit while loop
		}

		// handle pwd
		if (strcmp(splitCmd[0], "pwd") == 0) {
			getcwd(cmdline, MAXBUF);
			printf("%s\n", cmdline);
		}

		freeSplitResult(splitCmd); // free split command pieces before re-printing david shell prompt
	}

	return 0;
}
