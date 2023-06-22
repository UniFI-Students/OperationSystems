#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include "BreakByWire.h"
#include "../Logger/Logger.h"
#include "../DateProvider/DateProvider.h"

#define BRAKE "FRENO"
#define STOP_CAR "ARRESTO AUTO"
#define BREAK_BY_WIRE_LOGFILE "brake.log"
#define BREAK_BY_WIRE_ERROR_LOGFILE "brake.eLog"

void handleBrakeCommand(BrakeByWireCommand command);

ssize_t receiveCommandFromEcu(BrakeByWireCommand *pCommand);
ssize_t receiveStopSignalFromEcu();

void handleStopSignal();

void registerSignalHandlers();
void closeFileDescriptors();

int main(){
    BrakeByWireCommand cmd;
    setLogFileName(BREAK_BY_WIRE_LOGFILE);
    setErrorLogFileName(BREAK_BY_WIRE_ERROR_LOGFILE);
    instantiateLogFileDescriptor();
    instantiateErrorLogFileDescriptor();

    registerSignalHandlers();
    while(1)
    {
        receiveCommandFromEcu(&cmd);
        handleBrakeCommand(cmd);
    }
}

void registerSignalHandlers() {
    signal(SIGUSR1, handleStopSignal);
    signal(SIGINT, closeFileDescriptors);
}

void closeFileDescriptors(){
    closeLogFileDescriptor();
    closeErrorLogFileDescriptor();
}


ssize_t receiveCommandFromEcu(BrakeByWireCommand *pCommand) {
    //TODO: Implement receiveCommandFromEcu
    pCommand->type = -1;
}

void handleBrakeCommand(BrakeByWireCommand command) {
    char message[32];
    char *commandType;

    switch (command.type) {

        case Brake:
            commandType = BRAKE;
            break;
        default:
            return;
    }

    sprintf(message, "%s %s %lu", getCurrentDateTime(), commandType, command.quantity);
    logMessage(message);
}

void handleStopSignal() {
    logMessage(STOP_CAR);
}
