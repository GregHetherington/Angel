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

bool compareFirstName(const void *first, const void *second){

    if (first == NULL || second == NULL){
        return false;
    }             
                    //Elements are "equal" if their first names are equal
    if (strcmp(first, second) == 0) {
        return true;
    } else {
        return false;
    }
}

int main(void){
	//Allocate the strings
	char* str[4];
	
	for (int i = 0; i < 4; i++){
		str[i] = (char*)malloc(10*sizeof(char));
	}
	strcpy(str[0], "Hello");
	strcpy(str[1], " ");
	strcpy(str[2], "world");
	strcpy(str[3], "!");
	
	/* 
	Create the list.  The list is allocated on the stack, and initializeList returns the list struct.
	*/
	List list;
	printf("len: %d\n", getLength(list));
	list = initializeList(&printFunc, &deleteFunc, &compareFunc);
    printf("len: %d\n", getLength(list));
	//Populate the list
	for (int i = 0; i < 4; i++){
		insertBack(&list, (void*)str[i]); printf("len: %d\n", getLength(list));
	}
	
	void* elem;
	
	//Create an iterator - again, the iterator is allocated on the stack
	ListIterator iter = createIterator(list);

	/*
	Traverse the list using an iterator.  
	nextElement() returns NULL ones we reach the end of the list
	*/

	while ((elem = nextElement(&iter)) != NULL){
		char* str = (char*)elem;
		//Since str is the pionter to the stored string, we don't need to free it
		printf("%s", str);
	}

    char* data = findElement(list, &compareFirstName, "Hello");
    printf("\nSearchTerm: %s", data);

	printf("\n");
	
	/*
	Crear list contents - free each node, including its contents
	Since the list is created in the stack, we don't need to free it. 
	*/
	clearList(&list);	
	
	return 0;		
}











