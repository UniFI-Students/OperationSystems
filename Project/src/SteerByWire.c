#include <unistd.h>
#include "SteerByWire.h"
#include "Logger.h"

#define STEERING_LEFT "STO GIRANDO A SINISTRA"
#define STEERING_RIGHT "STO GIRANDO A DESTRA"
#define NO_ACTION "NO ACTION"
#define STEER_BY_WIRE_LOGFILE "steer.logMessage"

void receiveCommandFromEcu(SteerByWireCommand *steerCommandResultPointer);

void handleSteerRightCommand();
void handleNoActionCommand();
void handleSteerLeftCommand();


int main(void){
    SteerByWireCommand steerCommand;
    void(*steerHandleFuncHashMap[3])();

    steerHandleFuncHashMap[Left % 3] = &handleSteerLeftCommand;
    steerHandleFuncHashMap[Right % 3] = &handleSteerLeftCommand;
    steerHandleFuncHashMap[NoAction % 3] = &handleSteerLeftCommand;


    while(1) {
        receiveCommandFromEcu(&steerCommand);

        for (int i = 0; i < 4; ++i) {

            steerHandleFuncHashMap[steerCommand.type % 3]();
            sleep(1);
        }
    }
}

void handleSteerLeftCommand() {
    logMessage(STEER_BY_WIRE_LOGFILE, STEERING_LEFT);
}


void handleNoActionCommand() {
    logMessage(STEER_BY_WIRE_LOGFILE, STEERING_RIGHT);
}

void handleSteerRightCommand() {
    logMessage(STEER_BY_WIRE_LOGFILE, NO_ACTION);
}

void receiveCommandFromEcu(SteerByWireCommand *steerCommandResultPointer) {
//TODO: Implement receiveCommandFromEcu
}
