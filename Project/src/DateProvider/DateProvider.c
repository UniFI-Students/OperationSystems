#include <time.h>
#include <string.h>
#include "DateProvider.h"

char* getCurrentDateTime(){
    time_t t;
    time(&t);
    return strtok(ctime(&t),"\n");
}