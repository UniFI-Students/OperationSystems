#include "Random.h"
#include <stdlib.h>

void startRand(unsigned seed){
    srand(seed);
}

double randDouble(double min, double max){
    double range = (max - min);
    double d = RAND_MAX / range;
    return min + (rand() / d);
}