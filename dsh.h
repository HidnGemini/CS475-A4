#include <stdbool.h>

#define MAXBUF 256  // max number of characteres allowed on command line

char** split(char*, char*); // split string into multiple strings along the delimiter
void freeSplitResult(char**); // frees all memory allocated created by split
void runCmd(char**, int); // runs the command given (in split form) assuming full path provided and command exists
bool hasDoubleSpaces(char*); // returns true if input has two (or more) consecutive spaces
bool isOnlySpaces(char*); // returns true if the passed string is only spaces
void trimSpaces(char*); // removes leading spaces, double spaces, and trailing spaces from passed string
void removeDoubleSpaces(char*); // replaces double spaces with a single space (assuming no leading or trailing spaces)
void cleanUpString(char*); // runns trimSpaces and removeDoubleSpaces
