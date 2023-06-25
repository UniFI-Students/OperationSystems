#ifndef OS_CENTRAL_ECU_H
#define OS_CENTRAL_ECU_H

#define CENTRAL_ECU_LOGFILE "ECU.log"
#define CENTRAL_ECU_ERROR_LOGFILE "ECU.eLog"

enum CarState {
    CarStateNone,
    CarStateStarted,
    CarStatePreparingToPark,
    CarStateParking
} typedef CarState;


#endif
