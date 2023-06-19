#include <time.h>
#include "DateProvider.h"

char* getCurrentDateTime(){
    time_t t;
    time(&t);
    return ctime(&t);
}