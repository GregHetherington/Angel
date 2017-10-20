/*

* Cis2750 F2017

* Assignment 1

* Greg Hetherington 0936504

* file contains a Parser for the iCalendar file format

* Resources:

    -Data Structures (Cis2520) with fangji Wang
    -Cis2750 Lectures with Denis Nikitenko
    -Stackoverflow.com
    -tutoriaalpoint.com

*/

#include "CalendarParser.h"
#include <ctype.h>

int isDateTimeMalformed(char *dateTime) {
    if (dateTime == NULL || strcmp(dateTime, "") == 0) {
        return 0;
    }
    if (dateTime[8] != 'T') {
        return 0;
    }
    for (int i = 0; i < strlen(dateTime); i++) {
        if (i < 8) {
            if (!isdigit(dateTime[i])) {
                return 0;
            }
        } else if (i > 8 && i < 15) {
            if (!isdigit(dateTime[i])) {
                return 0;
            }
        }
    }
    return 1;
}

int isVersionMalformed(char * version) {
    if (version == NULL || strcmp(version, "") == 0) {
        return 0;
    }
    int numberOfDecimals = 0;
    for (int i = 0; i < strlen(version); i++) {
        if (!isdigit(version[i]) && version[i] != '.') {
            return 0;
        } else if (version[i] == '.') {
            numberOfDecimals++;
            if (numberOfDecimals > 1) {
                return 0;
            }
        }
    }
    return 1;
}

char* getFileExtention(char *fileName) {
    char *fileExt = strrchr(fileName, '.');
    if (!fileExt || fileExt == fileName) {
        return "";
    }
    return fileExt + 1;
}

void addEndToString(char *str) {
    int count = 0;
    for (int i = 0; str[i]; i++) {
        count++;
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

int getFileLenght(char *fileName) {
    int fileLenght = 0;
    if (fileName == NULL) {
        return 0;
    }
    FILE *f;
    f = fopen(fileName, "r");
    char line[128];
    if (f == NULL) {
        return 0;
    }                                                
    while (fgets(line, sizeof(line), f) != NULL) {
        for (int i = 0; line[i] != '\0'; i++) {
            if (line[i] == ';') {
                break;
            } else if (line[i] != ' ') {
                addEndToString(line);
                if (isStringOnlySpaces(line) == 0) {
                    fileLenght++;
                }
                break;
            }
        }
    } fclose(f);
    return fileLenght;
}

void propDestroy(void *data) {
    free(data);
}
void alarmDestroy(void *data) {
    Alarm* tmpAlarm;
    tmpAlarm = (Alarm*)data;
    clearList(&tmpAlarm->properties);
    free(tmpAlarm->trigger);
    free(tmpAlarm);
}
void eventDestroy(void *data) {
    Event* tmpEvent;
    tmpEvent = (Event*)data;
    clearList(&tmpEvent->properties);
    clearList(&tmpEvent->alarms);
    free(tmpEvent);
}
char * printAlarm(void *toBePrinted) {
    char* tmpStr;
    Alarm* tmpName;
    int len;
            
    if (toBePrinted == NULL) {
        return NULL;
    }
                            
    tmpName = (Alarm*)toBePrinted;
                               
    len = strlen(tmpName->action)+strlen(tmpName->trigger)+40;
    tmpStr = (char*)malloc(sizeof(char)*len);

    sprintf(tmpStr, "Action:%s Trigger:%s\n", tmpName->action, tmpName->trigger);
    return tmpStr;
}
char * printProp(void *toBePrinted) {
    char* tmpStr;
    Property* tmpName;
    int len;
    
    if (toBePrinted == NULL) {
        return NULL;
    }
    
    tmpName = (Property*)toBePrinted;

    len = strlen(tmpName->propName)+strlen(tmpName->propDescr)+40;
    tmpStr = (char*)malloc(sizeof(char)*len);
    
    sprintf(tmpStr, "Name:%s Descr:%s\n", tmpName->propName, tmpName->propDescr);
    
    return tmpStr;
}
char * printEvent(void *toBePrinted) {
    char* tmpStr;
    Event* tmpName;
    int len;
            
    if (toBePrinted == NULL) {
        return NULL;
    }
                            
    tmpName = (Event*)toBePrinted;
                                
    len = strlen(tmpName->UID)+strlen(tmpName->creationDateTime.date)+strlen(tmpName->creationDateTime.time)+40;
    tmpStr = (char*)malloc(sizeof(char)*len);
    sprintf(tmpStr, "UID:%s Date:%s Time:%s\n", tmpName->UID, tmpName->creationDateTime.date, tmpName->creationDateTime.time);

    return tmpStr;
}
int testCompare(const void * one, const void * two) {
    return strcmp((char*)one, (char*)two);
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
    if (fileName == NULL || strcmp(getFileExtention(fileName), "ics") != 0) {
        return INV_FILE;
    }
    FILE *iCalFile;
    iCalFile = fopen(fileName, "r");        
    
    char lineParse[2000];
    if (iCalFile == NULL) {
        return INV_FILE;
    }
    int k = 0;
    int fileLenght = getFileLenght(fileName);
    char** fileContentsData = malloc(fileLenght * sizeof(char *));//malloc
    char** fileContentsType = malloc(fileLenght * sizeof(char *));//malloc

    char** line = malloc(fileLenght * sizeof(char *));

    while (fgets(lineParse, sizeof(lineParse), iCalFile) != NULL) {
        if (lineParse[0] == ';') {
            break;
        } else if (lineParse[0] == ' ') {
            fileLenght--;
            addEndToString(lineParse);
            line[k-1] = realloc(line[k-1], (strlen(lineParse) * sizeof(char)) + strlen(line[k-1]));
            char tmp[75];
            sprintf(tmp, "%s", lineParse);
            strcpy(tmp, tmp + 1);
            strcat(line[k-1], tmp);
        } else if (lineParse[0] != ' ') {
            addEndToString(lineParse);
            line[k] = malloc((strlen(lineParse) + 1) * sizeof(char));
            strcpy(line[k], lineParse);
            k++;
        }
        //printf("line:{%s}\n", line[k-1]);
    }
    
    k = 0;
    while (line[k] != NULL) {
        for (int i = 0; line[k][i] != '\0'; i++) {
            if (line[k][i] == ';') {
                break;
            } else if (line[k][i] != ' ') {
                if (isStringOnlySpaces(line[k]) == 0) {
                    char* type = malloc((strlen(line[k]) + 1) * sizeof(char));//malloc
                    char* data = malloc((strlen(line[k]) + 1) * sizeof(char));//malloc
                    int c = 0;
                    for(int i = 0; i <= strlen(line[k]); i++) {
                        if (c != 0 && line[k][i] != '\0') {
                            data[i-c-1] = line[k][i];
                        } else if ((line[k][i] == ';' || line[k][i] == ':') && c == 0) {
                            type[i] = '\0';
                            c = i;
                        } else if (line[k][i] == '\0') {
                            data[i-c-1] = '\0';
                            break;
                        } else {
                            line[k][i] = toupper(line[k][i]);
                            type[i] = line[k][i];
                        }
                    }
                    fileContentsType[k] = malloc((strlen(type) + 1) * sizeof(char));
                    strcpy(fileContentsType[k], type);
                    
                    fileContentsData[k] = malloc((strlen(data) + 1) * sizeof(char));
                    strcpy(fileContentsData[k], data);
                                        
                    //printf("{%s}\n", line[k]);
                    //printf("Type: {%s},Data: {%s}\n", fileContentsType[k], fileContentsData[k]);
                    
                    k++;
                    free(type);
                    free(data);
                }
                break;
            }
        }
    }
    fclose(iCalFile);
    
    printf("%s\n", fileContentsData[0]);
    
    if (strcmp(fileContentsType[0], "BEGIN") != 0 || strcmp(fileContentsData[0], "VCALENDAR") != 0) {
        for (int i = 0; i < fileLenght; i++) {
            free(fileContentsType[i]);
            free(fileContentsData[i]);
        }
        for (int i = 0; line[i]; i++) {
            free(line[i]);
        }
        free(fileContentsType);
        free(fileContentsData);
        free(line);        
        return INV_CAL;
    }
    
    Calendar* cal = malloc(sizeof(Calendar));
    
    ErrorCode returnCode = INV_CAL;
    int isTrigger = 0;
    char **state = malloc(10 * sizeof(char *));//malloc
    int stateNum = 0;
    state[stateNum] = "NONE";

    for (int i = 0; i < fileLenght; i++) {
        printf("State:{%s},Type:{%s},Data:{%s}\n", state[stateNum], fileContentsType[i], fileContentsData[i]);
        if (strcmp(fileContentsType[i], "BEGIN") == 0) {
            stateNum++;
            state[stateNum] = malloc((strlen(fileContentsData[i]) + 1) * sizeof(char));
            if (strcmp(fileContentsData[i], "VCALENDAR") == 0) {
                strcpy(state[stateNum], "VCALENDAR");
                cal->version = -1;
                strcpy(cal->prodID, "-1");
                cal->events = initializeList(printEvent, eventDestroy, testCompare);
                cal->properties = initializeList(printProp, propDestroy, testCompare);
            } else if (strcmp(fileContentsData[i], "VEVENT") == 0) {
                strcpy(state[stateNum], "VEVENT");
                Event * newEvent = malloc(sizeof(Event));
                strcpy(newEvent->UID, "-1");
                strcpy(newEvent->creationDateTime.time, "-1");
                strcpy(newEvent->creationDateTime.date, "-1");
                newEvent->properties = initializeList(printProp, propDestroy, testCompare);
                newEvent->alarms = initializeList(printAlarm, alarmDestroy, testCompare);
                insertBack(&cal->events, newEvent);
            } else if (strcmp(fileContentsData[i], "VALARM") == 0) {
                strcpy(state[stateNum], "VALARM");
                Alarm * newAlarm = malloc(sizeof(Alarm));
                strcpy(newAlarm->action, "-1");
                newAlarm->properties = initializeList(printProp, propDestroy, testCompare);
                Event * event = getFromBack(cal->events);
                insertBack(&event->alarms, newAlarm);
                isTrigger = 0;
            } else {
                strcpy(state[stateNum], "OTHER");
            }
        } else if (strcmp(fileContentsType[i], "END") == 0) {
                if (strcmp(fileContentsData[i], "VCALENDAR") == 0 && strcmp(state[stateNum], "VCALENDAR") == 0) {
                    //check for missing proporties
                    if (returnCode == INV_CAL) {
                        returnCode = OK;
                        if (cal->version == -1 || strcmp(cal->prodID, "-1") == 0 || getFromBack(cal->events) == NULL) {
                            returnCode = INV_CAL;
                        }
                    }
                    free(state[stateNum]);
                    stateNum--;
                } else if (strcmp(fileContentsData[i], "VEVENT") == 0 && strcmp(state[stateNum], "VEVENT") ==0) {
                    //checkfor missing properties
                    Event * event = getFromBack(cal->events);
                    if (returnCode == INV_CAL) {
                        if (strcmp(event->UID, "-1") == 0 || strcmp(event->creationDateTime.time, "-1") == 0 || strcmp(event->creationDateTime.date, "-1") == 0) {
                            returnCode = INV_EVENT;printf("vE: %s:%s:%s\n", event->UID, event->creationDateTime.time, event->creationDateTime.date);
                        }
                    }
                    free(state[stateNum]);
                    stateNum--;
                } else if (strcmp(fileContentsData[i], "VALARM") == 0 && strcmp(state[stateNum], "VALARM") ==0) {
                    //checkfor missing properties
                    Event * event = getFromBack(cal->events);
                    Alarm * alarm = getFromBack(event->alarms);
                    if (returnCode == INV_CAL) {
                        if (strcmp(alarm->action, "-1") == 0 || isTrigger == 0) {
                            returnCode = INV_EVENT;printf("vA\n");
                        }
                    }
                    free(state[stateNum]);
                    stateNum--;
                } else if (strcmp(state[stateNum], "OTHER") == 0) {
                    free(state[stateNum]);
                    stateNum--;                    
                } else {
                    returnCode = INV_CAL;
                }
        } else if (strcmp(fileContentsType[i], "VERSION") == 0 && strcmp(state[stateNum], "VCALENDAR") == 0) {//in create vcal state
            //set version
            if(cal->version != -1) {
                returnCode = DUP_VER;
            } else if (isVersionMalformed(fileContentsData[i]) == 0) {
                returnCode = INV_VER;
            } else {
                cal->version = atof(fileContentsData[i]);
            }
        } else if (strcmp(fileContentsType[i], "PRODID") == 0 && strcmp(state[stateNum], "VCALENDAR") == 0) {
            if(strcmp(cal->prodID, "-1") != 0) {
                returnCode = DUP_PRODID;
            } else if (strcmp(fileContentsData[i], "") == 0 || fileContentsData == NULL) {
                returnCode = INV_PRODID;
            } else {
                strcpy(cal->prodID, fileContentsData[i]);
            }
        } else if (strcmp(fileContentsType[i], "UID") == 0 && strcmp(state[stateNum], "VEVENT") == 0) {
            //set UID
            Event * event = getFromBack(cal->events);
            strcpy(event->UID, fileContentsData[i]);
        } else if (strcmp(fileContentsType[i], "DTSTAMP") == 0 && strcmp(state[stateNum], "VEVENT") == 0) {
            //set DTSTAMP
            if (isDateTimeMalformed(fileContentsData[i]) == 0) {
                returnCode = INV_CREATEDT;
            } else {
                char date[9];
                char time[7];
                strncpy(date, fileContentsData[i], 8);
                date[8] = '\0';
                char* p = strchr(fileContentsData[i], 'T');
                for(int i = 1; i < 7; i++) {
                    time[i-1] = p[i];
                    time[i] = '\0';
                }
                Event * event = getFromBack(cal->events);
                if (p[7] == 'Z') {
                    event->creationDateTime.UTC = true;
                } else {
                    event->creationDateTime.UTC = false;
                }
                strcpy(event->creationDateTime.time, time);
                strcpy(event->creationDateTime.date, date);
            }
        } else if (strcmp(fileContentsType[i], "ACTION") == 0 && strcmp(state[stateNum], "VALARM") == 0) {
            Event * event = getFromBack(cal->events);
            Alarm * alarm = getFromBack(event->alarms);
            strcpy(alarm->action, fileContentsData[i]);
        } else if (strcmp(fileContentsType[i], "TRIGGER") == 0 && strcmp(state[stateNum], "VALARM") == 0) {
            Event * event = getFromBack(cal->events);
            Alarm * alarm = getFromBack(event->alarms);
            alarm->trigger = malloc(sizeof(char) * (strlen(fileContentsData[i]) + 1));
            strcpy(alarm->trigger, fileContentsData[i]);
            isTrigger = 1;
        } else if (strcmp(state[stateNum], "VALARM") == 0) {//parsing alarm properties
            Event * event = getFromBack(cal->events);
            Alarm * alarm = getFromBack(event->alarms);
            Property * prop = malloc(sizeof(Property) + ((1 + strlen(fileContentsData[i])) *sizeof(char)));
            if (strcmp(fileContentsType[i], "") == 0 || fileContentsType[i] == NULL || strcmp(fileContentsData[i], "") == 0 || fileContentsData[i] == NULL) {
                returnCode = INV_EVENT;
            }
            strcpy(prop->propName, fileContentsType[i]);
            strcpy(prop->propDescr, fileContentsData[i]);
            insertBack(&alarm->properties, prop);
        } else if (strcmp(state[stateNum], "VEVENT") == 0) {//parsing event properties            
            Event * event = getFromBack(cal->events);
            Property * prop = malloc(sizeof(Property) + sizeof(char)*(1 + strlen(fileContentsData[i])));
            if (strcmp(fileContentsType[i], "") == 0 || fileContentsType[i] == NULL || strcmp(fileContentsData[i], "") == 0 || fileContentsData[i] == NULL) {
                returnCode = INV_EVENT;
            }
            strcpy(prop->propName, fileContentsType[i]);
            strcpy(prop->propDescr, fileContentsData[i]);
            insertBack(&event->properties, prop);
        } else if (strcmp(state[stateNum], "VCALENDAR") == 0) {//parsing vcalendar properties
            Property * prop = malloc(sizeof(Property) + sizeof(char)*(1 + strlen(fileContentsData[i])));
            if (strcmp(fileContentsType[i], "") == 0 || fileContentsType[i] == NULL || strcmp(fileContentsData[i], "") == 0 || fileContentsData[i] == NULL) {
                returnCode = INV_EVENT;
            }
            strcpy(prop->propName, fileContentsType[i]);
            strcpy(prop->propDescr, fileContentsData[i]);
            insertBack(&cal->properties, prop);
        }
    }
    
    if (returnCode != OK) {
        deleteCalendar(cal);
    } else {
        *obj = cal;
    }
    
    for (int i = 0; i < fileLenght; i++) {
        free(fileContentsType[i]);
        free(fileContentsData[i]);
    }
    for (int i = 0; line[i]; i++) {
        free(line[i]);
    }
    free(fileContentsType);
    free(fileContentsData);
    free(line);
    free(state);    

    return returnCode;
}


/** Function to delete all calendar content and free all the memory.
 *@pre Calendar object exists, is not null, and has not been freed
 *@post Calendar object had been freed
 *@return none
 *@param obj - a pointer to a Calendar struct
**/

void deleteCalendar(Calendar* obj) {
    clearList(&obj->properties);
    clearList(&obj->events);
    free(obj);
}



/** Function to create a string representation of a Calendar object.
 *@pre Calendar object exists, is not null, and is valid
 *@post Calendar has not been modified in any way, and a string representing the Calndar contents has been created
 *@return a string contaning a humanly readable representation of a Calendar object
 *@param obj - a pointer to a Calendar struct
**/

char* printCalendar(const Calendar* obj) {
    char* printStr;
    char* tmpStr;
        
    if (obj == NULL) {
        return "list is null\n";
    }
    
    tmpStr = (char*)malloc(sizeof(char)*1000);
    printStr = (char*)malloc(sizeof(char)*10000);

    sprintf(tmpStr, "Calendar: version = %f, prodID = %s\n", obj->version, obj->prodID);
    strcat(printStr, tmpStr);

    sprintf(tmpStr, "Events:\n");
    strcat(printStr, tmpStr);
    
    if (obj->events.head != NULL) {
        Event* elemE;
        ListIterator iterE = createIterator(obj->events);

        while ((elemE = nextElement(&iterE)) != NULL) {
            Event * event = elemE;

            sprintf(tmpStr, "\n\tEvent:\tUID = %s\n", event->UID);
            strcat(printStr, tmpStr);
    
            int UTC = 0;
            if (event->creationDateTime.UTC) {
                UTC = 1;
            }
            sprintf(tmpStr, "\tcreationDateTime = %s:%s, UTC=%d\n", event->creationDateTime.date, event->creationDateTime.time, UTC);
            strcat(printStr, tmpStr);
            //alarm

            if (event->alarms.head != NULL) {    
                Alarm* elemA;
                ListIterator iterA = createIterator(event->alarms);

                sprintf(tmpStr, "\tAlarms:\n");
                strcat(printStr, tmpStr);

                while ((elemA = nextElement(&iterA)) != NULL) {
                    Alarm * alarm = elemA;
                    sprintf(tmpStr, "\t\tAction: %s\n\t\tTrigger: %s\n", alarm->action, alarm->trigger);
                    strcat(printStr, tmpStr);
                    //alarm properties
                    sprintf(tmpStr, "\t\tProperties:\n");
                    strcat(printStr, tmpStr);

                    if (alarm->properties.head != NULL) {
                        Property* elem;
                        ListIterator iter = createIterator(alarm->properties);

                        while ((elem = nextElement(&iter)) != NULL) {
                            Property * prop = elem;
                            sprintf(tmpStr, "\t\t- %s:%s\n", prop->propName, prop->propDescr);
                            strcat(printStr, tmpStr);
                        }
                        sprintf(tmpStr, "\n");
                        strcat(printStr, tmpStr);
                    }
                }
            }
            //event properties
            sprintf(tmpStr, "\n\tOther Event properties:\n");
            strcat(printStr, tmpStr);

            if (event->properties.head != NULL) {
                Property* elem;
                ListIterator iter = createIterator(event->properties);

                while ((elem = nextElement(&iter)) != NULL) {
                    Property * prop = elem;
                    sprintf(tmpStr, "\t\t- %s:%s\n", prop->propName, prop->propDescr);
                    strcat(printStr, tmpStr);
                }
            }
        }
    }
    
    //event properties
    sprintf(tmpStr, "\n\tOther iCal properties:\n");
    strcat(printStr, tmpStr);
             
    if (obj->properties.head != NULL) {
        Property* elem;
        ListIterator iter = createIterator(obj->properties);

        while ((elem = nextElement(&iter)) != NULL) {
            Property * prop = elem;
            sprintf(tmpStr, "\t\t- %s:%s\n", prop->propName, prop->propDescr);
            strcat(printStr, tmpStr);
        }
    }
    
    free(tmpStr);
    return printStr;
}

const char* printError(ErrorCode err) {
    if (err == OK) {
        return "OK";
    } else if (err == INV_FILE) {
        return "INV_FILE";
    } else if (err == INV_CAL) {
        return "INV_CAL";
    } else if (err == INV_VER) {
        return "INV_VER";
    } else if (err == INV_PRODID) {
        return "INV_PRODID";
    } else if (err == DUP_VER) {
        return "DUP_VER";
    } else if (err == DUP_PRODID) {
        return "DUP_PRODID";
    } else if (err == INV_EVENT) {
        return "INV_EVENT";
    } else if (err == INV_CREATEDT) {
        return "INV_CREATEDT";
    } else if (err == OTHER_ERROR) {
        return "OTHER_ERROR";
    }
    return "OTHER_ERROR";
}

/** Function to writing a Calendar object into a file in iCalendar format.
 *@pre Calendar object exists, is not null, and is valid
  *@post Calendar has not been modified in any way, and a file representing the
          Calendar contents in iCalendar format has been created
           *@return the error code indicating success or the error encountered when parsing the calendar
            *@param obj - a pointer to a Calendar struct
             **/
ErrorCode writeCalendar(char* fileName, const Calendar* obj) {

    


}

/** Function to validating an existing a Calendar object
 *@pre Calendar object exists and is not null
  *@post Calendar has not been modified in any way
   *@return the error code indicating success or the error encountered when validating the calendar
    *@param obj - a pointer to a Calendar struct
     **/
//ErrorCode validateCalendar(const Calendar* obj);
