#include "CalendarParser.h"

int main(void) {

    Calendar* testCal;
    ErrorCode err = createCalendar("test.ics", &testCal);
    printf("Errorcode: %s\n", printError(err));
    printf("\n\n%s\n", printCalendar(testCal));

    return 0;
}
