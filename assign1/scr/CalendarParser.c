#include "../include/CalendarParser.h"

void removeSpacesFromfrontOfString(char *str) {
    int count = 0;
    int isPastFront = 0;
    for (int i = 0; str[i]; i++) {
        if (str[i] != ' ') {
            isPastFront = 1;
            str[count++] = str[i];
        } else if (isPastFront == 1) {
            str[count++] = str[i];
        }
    }
    str[count - 1] = '\0';
}

int isStringOnlySpaces(char *str) {
    int spaceCount = 0;
    for (int i = 0; str[i]; i++) {
        if (str[i] == ' ') {
            spaceCount++;
        }
    }
    if (spaceCount == strlen(str)) {
        return 1;
    } else {
        return 0;
    }
}

/** Function to create a Calendar object based on the contents of an iCalendar file.
 *@pre File name cannot be an empty string or NULL.  File name must have the .ics extension.
       File represented by this name must exist and must be readable.
 *@post Either:
        A valid calendar has been created, its address was stored in the variable obj, and OK was returned
		or 
		An error occurred, the calendar was not created, all temporary memory was freed, obj was set to NULL, and the 
		appropriate error code was returned
 *@return the error code indicating success or the error encountered when parsing the calendar
 *@param fileName - a string containing the name of the iCalendar file
 *@param a double pointer to a Calendar struct that needs to be allocated
**/
ErrorCode createCalendar(char* fileName, Calendar** obj) {
    //read file
    if (fileName == NULL) {
        return INV_FILE;
    }
    
    FILE *iCalFile;
    iCalFile = fopen(fileName, "r");        
    
    char line[128];
    if (iCalFile == NULL) {
        return INV_FILE;
    }
    int k = 0;
    char** fileContentsData = malloc(100 * sizeof(char *));
    char** fileContentsType = malloc(100 * sizeof(char *));
    
    while (fgets(line, sizeof(line), iCalFile) != NULL) {
        for (int i = 0; line[i] != '\0'; i++) {
            if (line[i] == ';') {
                break;
            } else if (line[i] != ' ') {
                removeSpacesFromfrontOfString(line);
                if (isStringOnlySpaces(line) == 0) {
                    char *p = strtok(line, ":");
                    fileContentsType[k] = (char *)malloc(sizeof(p) + 1);
                    strcpy(fileContentsType[k], p);
                    p = strtok(NULL, ":");
                    fileContentsData[k] = (char *)malloc(sizeof(p) + 1);
                    strcpy(fileContentsData[k], p);
                    k++;
                }
                break;
            }
        }
    }
    fclose(iCalFile);
    for (int i = 0; fileContentsType[i] != NULL; i++) {
        printf("Type:{%s},Data:{%s}\n", fileContentsType[i], fileContentsData[i]);
        if (strcmp(fileContentsType[i], "BEGIN") == 0) {
            if (strcmp(fileContentsData[i], "VCALENDAR") == 0) {
                //state is create vcal
            } else if (strcmp(fileContentsData[i], "VEVENT") == 0) {
                //state is create vevent
            }
        } else if (strcmp(fileContentsType[i], "VERSION") == 0) {//in create vcal state & version
            //set version
            (**obj).version = atof(fileContentsData[i]);
        } else if (strcmp(fileContentsType[i], "PRODID") == 0) {
            //set prod id
        }
    }

    //add to Calendar
    //(**obj).version = 0;
    
    
    
    
    
    return OK;
}


/** Function to delete all calendar content and free all the memory.
 *@pre Calendar object exists, is not null, and has not been freed
 *@post Calendar object had been freed
 *@return none
 *@param obj - a pointer to a Calendar struct
**/

void deleteCalendar(Calendar* obj) {

}



/** Function to create a string representation of a Calendar object.
 *@pre Calendar object exists, is not null, and is valid
 *@post Calendar has not been modified in any way, and a string representing the Calndar contents has been created
 *@return a string contaning a humanly readable representation of a Calendar object
 *@param obj - a pointer to a Calendar struct
**/

char* printCalendar(const Calendar* obj) {
    char* printString = malloc(1000);
    
    //char* version = obj->version;
    //snprintf(printString, 1000, "%f", obj->version);
    
    //strcat(printString, version);
    
    return printString;
}



/** Function to "convert" the ErrorCode into a humanly redabale string.
 *@return a string contaning a humanly readable representation of the error code by indexing into 
          the descr array using rhe error code enum value as an index
 *@param err - an error code
**/
/*const char* printError(ErrorCode err) {

}
*/

int main (void) {
    char * str = "testiCal.ics";
    Calendar iCal;
    Calendar *pointer = &iCal;
    Calendar **data = &pointer;
    ErrorCode temp = createCalendar(str, data);
    if(temp == OK) {
        printf("it's gud\n");
    } else {
        printf("not gud\n");
    }
    
}
