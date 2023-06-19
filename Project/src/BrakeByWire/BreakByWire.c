#include <unistd.h>
#include <stdio.h>
#include "BreakByWire.h"
#include "../Logger/Logger.h"
#include "../DateProvider/DateProvider.h"

#define BRAKE "FRENO"
#define STOP_CAR "ARRESTO AUTO"
#define BREAK_BY_WIRE_LOGFILE "brake.log"

void handleBrakeCommand(BrakeByWireCommand command);

ssize_t receiveCommandFromEcu(BrakeByWireCommand *pCommand);
ssize_t receiveStopSignalFromEcu();

void handleStopSignal();

int main(){
    BrakeByWireCommand cmd;
    while(1)
    {
        if (receiveCommandFromEcu(&cmd)>=0)
            handleBrakeCommand(cmd);
        if (receiveStopSignalFromEcu() >= 0)
            handleStopSignal();
    }
}



ssize_t receiveStopSignalFromEcu() {
    //TODO: Implement receiveStopSignalFromEcu
    return 0;
}

ssize_t receiveCommandFromEcu(BrakeByWireCommand *pCommand) {
    //TODO: Implement receiveCommandFromEcu
}

void handleBrakeCommand(BrakeByWireCommand command) {
    char message[32];
    char *commandType;

    switch (command.type) {

        case Brake:
            commandType = BRAKE;
            break;
    }

    sprintf(message, "%s %s %lu", getCurrentDateTime(), commandType, command.quantity);
    logMessage(BREAK_BY_WIRE_LOGFILE, message);
}

void handleStopSignal() {
    logMessage(BREAK_BY_WIRE_LOGFILE, STOP_CAR);
}
