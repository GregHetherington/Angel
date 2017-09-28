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
                    char* type = malloc((strlen(line) + 1) * sizeof(char));
                    char* data = malloc((strlen(line) + 1) * sizeof(char));
                    int c = 0;
                    for(int i = 0; i <= strlen(line); i++) {
                        if (c != 0 && line[i] != '\0') {
                            data[i-c-1] = line[i];
                        } else if (line[i] == ':' && c == 0) {
                            type[i] = '\0';
                            c = i;
                        } else if (line[i] == '\0') {
                            data[i-c-1] = '\0';
                        } else {
                            type[i] = line[i];
                        }
                    }
                    //printf("T:%s D:%s\n", type, data);
                    fileContentsType[k] = malloc((strlen(type) + 1) * sizeof(char));
                    strcpy(fileContentsType[k], type);
                    
                    fileContentsData[k] = malloc((strlen(data) + 1) * sizeof(char));
                    strcpy(fileContentsData[k], data);
                    k++;
                    free(type);
                    free(data);
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
                (**obj).event = malloc(sizeof(Event));
            }
        } else if (strcmp(fileContentsType[i], "VERSION") == 0) {//in create vcal state
            //set version
            (**obj).version = atof(fileContentsData[i]);
        } else if (strcmp(fileContentsType[i], "PRODID") == 0) {//in create vcal state
            //set prod id
            strcpy((**obj).prodID, fileContentsData[i]);
        } else if (strcmp(fileContentsType[i], "UID") == 0) {//in create vevent state
            //set UID
            strcpy((**obj).event->UID, fileContentsData[i]);
        } else if (strcmp(fileContentsType[i], "DTSTAMP") == 0) {//in create vevent state
            //set DTSTAMP
            char date[9];
            char time[7];
            strncpy(date, fileContentsData[i], 8);
            date[8] = '\0';
            char* p = strchr(fileContentsData[i], 'T');
            for(int i = 1; i < strlen(p); i++) {
                time[i-1] = p[i];
                time[i] = '\0';
            }
            if (time[strlen(time) - 1] == 'Z') {
                (**obj).event->creationDateTime.UTC = true;
            } else {
                (**obj).event->creationDateTime.UTC = false;
            }
            //printf("D:{%s} T:{%s}\n", date, time);
            strcpy((**obj).event->creationDateTime.time, time);
            strcpy((**obj).event->creationDateTime.date, date);
        } else if (strcmp(fileContentsType[i], "ORGANIZER") == 0) {//in create vevent state
            //set organizer as a property
            strcpy((**obj).prodID, fileContentsData[i]);
        } else if (strcmp(fileContentsType[i], "DTSTART") == 0) {//in create vevent state
            //set DTSTART as a property
            strcpy((**obj).prodID, fileContentsData[i]);
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
