#include "../include/CalendarParser.h"
#include "LinkedListAPI.c"
#include "CalendarParser.c"
#include "../include/HelperFunctions.h"

int main (void) {
    char * str = "testiCal.ics";
    Calendar *iCal = malloc(sizeof(Calendar));
    Calendar **data = &iCal;
    ErrorCode temp = createCalendar(str, data);

    printf("ErrorCode:\n%s\n", printError(temp));

    char* printCal = printCalendar(iCal);
    printf("%s", printCal);
    free(printCal);

    deleteCalendar(iCal);
}
