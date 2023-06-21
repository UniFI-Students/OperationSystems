#include <unistd.h>
#include "SteerByWire.h"
#include "../Logger/Logger.h"

#define STEERING_LEFT "STO GIRANDO A SINISTRA"
#define STEERING_RIGHT "STO GIRANDO A DESTRA"
#define NO_ACTION "NO ACTION"

#define STEER_BY_WIRE_LOGFILE "steer.log"
#define STEER_BY_WIRE_ERROR_LOGFILE "steer.eLog"

void receiveCommandFromEcu(SteerByWireCommand *steerCommandResultPointer);

void handleSteerRightCommand();

void handleNoActionCommand();

void handleSteerLeftCommand();


int main(void) {
    SteerByWireCommand steerCommand;

    setLogFileName(STEER_BY_WIRE_LOGFILE);
    setErrorLogFileName(STEER_BY_WIRE_ERROR_LOGFILE);

    void (*steerHandleFuncHashMap[3])();

    steerHandleFuncHashMap[Left % 3] = &handleSteerLeftCommand;
    steerHandleFuncHashMap[Right % 3] = &handleSteerLeftCommand;
    steerHandleFuncHashMap[NoAction % 3] = &handleSteerLeftCommand;


    while (1) {
        receiveCommandFromEcu(&steerCommand);

        for (int i = 0; i < 4; ++i) {

            steerHandleFuncHashMap[steerCommand.type % 3]();
            sleep(1);
        }
    }
}

void handleSteerLeftCommand() {
    logMessage(STEERING_LEFT);
}


void handleNoActionCommand() {
    logMessage(STEERING_RIGHT);
}

void handleSteerRightCommand() {
    logMessage(NO_ACTION);
}

void receiveCommandFromEcu(SteerByWireCommand *steerCommandResultPointer) {
//TODO: Implement receiveCommandFromEcu
}
