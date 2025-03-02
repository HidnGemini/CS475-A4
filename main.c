/*
 * main.c
 *
 * runs a loop that prompts the user for a command and determines what
 * command to execute with that, reprompting the user if the command is
 * bad.
 *
 * Created on: Mar 3rd, 2025
 * Author: maddyS
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/wait.h>
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

		if (strlen(cmdline) > 1 && !isOnlySpaces(cmdline)) { // make sure at least one character given
			for (int i = 0; i<MAXBUF; i++) {
				if (cmdline[i] == '\n') {
					cmdline[i] = '\0';
					break; // shouldn't make any difference, but this makes valgrind happy :)
				}
			}
			cleanUpString(cmdline);

			char** splitCmd = split(cmdline, " ");

			// count num args
			int cmd_argc = 0;
			while (splitCmd[cmd_argc+1] != NULL) {
				cmd_argc++;
			}

			// ======================================
			// |			handle exit		  		|
			// ======================================
			if (strcmp(splitCmd[0], "exit") == 0) {
				free(cmdline); // free input command
				freeSplitResult(splitCmd); // free split command pieces and their pointer array

				break; // exit while loop
			}

			// ======================================
			// |			 handle pwd	  			|
			// ======================================
			else if (strcmp(splitCmd[0], "pwd") == 0) {
				getcwd(cmdline, MAXBUF);
				printf("%s\n", cmdline);
			}

			// ======================================
			// |			 handle cd	  			|
			// ======================================
			else if (strcmp(splitCmd[0], "cd") == 0) {
				if (chdir(splitCmd[1])) { // did i fail to change directory?
					printf("%s: no such file or directory\n", splitCmd[1]);
				}
			}

			// ======================================
			// |	handle mode 1 non-built ins		|
			// ======================================
			else if (splitCmd[0][0] == '/') {
				// full path to command
				if (access(splitCmd[0], F_OK | X_OK) == 0) { // does file exist? is it executable?
					runCmd(splitCmd, cmd_argc); // run program
				} else {
					printf("%s: command not found!\n", splitCmd[0]);
				}
			}

			// ======================================
			// |	handle mode 2 non-built ins		|
			// ======================================
			else {
				// get possible paths
				char* PATH = getenv("PATH");
				strcpy(cmdline, PATH); // use cmdline as buffer to avoid allocating more than necessary (and avoid overwriting PATH)
				char** splitPATH = split(cmdline, ":");

				// try each path
				int i = 0;
				bool didRun = false; // to print error afterwards
				while (splitPATH[i] != NULL) {
					strcpy(cmdline, splitPATH[i++]); // use cmdline as buffer to avoid allocating more than necessary
					strcat(cmdline, "/");
					strcat(cmdline, splitCmd[0]);
					if (access(cmdline, F_OK | X_OK) == 0) { // does this file exist? is it executable?
						strcpy(splitCmd[0], cmdline); // copy over full path
						runCmd(splitCmd, cmd_argc); // run program
						didRun = true;
					}
				}
				if (!didRun) {
					printf("%s: command not found!\n", splitCmd[0]);
				}

				freeSplitResult(splitPATH);
			}

			freeSplitResult(splitCmd); // free split command pieces before re-printing david shell prompt
		}
	}

	return 0;
}
