#include "CalendarParser.h"

int main(void) {

    Calendar* testCal;
    ErrorCode err = createCalendar("test.ics", &testCal);
    printf("Errorcode: %s\n", printError(err));

    return 0;
}
