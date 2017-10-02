#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include <ctype.h>

int isDateTimeMalformed(char *dateTime);

int isVersionMalformed(char * version);

char* getFileExtention(char *fileName);

int getFileLenght(char *fileName);

void removeSpacesFromfrontOfString(char *str);

int isStringOnlySpaces(char *str);

char* getFileExtention(char *fileName);

int isVersionMalformed(char * version);

#endif
