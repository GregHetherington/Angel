/* 
 * Simple list example - we store strings
 */

#include <stdio.h>
#include <string.h>
#include "LinkedListAPI.c"
#include "LinkedListAPI.h"

//Printing a string requires a simple cast
char* printFunc(void *toBePrinted){
	
	return (char*)toBePrinted;
}

//Comparing strings is done by strcmp
int compareFunc(const void *first, const void *second){
	
	return strcmp((char*)first, (char*)second);
}

//Freeing a string is also straightforward
void deleteFunc(void *toBeDeleted){
	
	free(toBeDeleted);
}

int main(void){
    printf("Start/n");
	//Allocate the strings
	char* str[4];
	
	for (int i = 0; i < 4; i++){
		str[i] = (char*)malloc(10*sizeof(char));
	}
	strcpy(str[0], "Hello");
	strcpy(str[1], " ");
	strcpy(str[2], "world");
	strcpy(str[3], "!");
	

}











