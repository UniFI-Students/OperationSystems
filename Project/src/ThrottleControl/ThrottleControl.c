#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "ThrottleControl.h"
#include "../Logger/Logger.h"
#include "../Random/Random.h"
#include "../DateProvider/DateProvider.h"

#define INCREMENT "INCREMENTO"

#define THROTTLE_CONTROL_LOGFILE "throttle.log"
#define THROTTLE_CONTROL_ERROR_LOGFILE "throttle.eLog"

#define FAIL_PROBABILITY 1e-5

void receiveCommandFromEcu(ThrottleControlCommand *pCommand);
void handleSucceedThrottleCommand(ThrottleControlCommand command);

bool isThrottleFailed();

void handleFailedThrottleCommand(ThrottleControlCommand command);

void signalAboutFailedThrottleCommandToEcu();

int main(){
    startRand(time(NULL));
    ThrottleControlCommand cmd;

    setLogFileName(THROTTLE_CONTROL_LOGFILE);
    setErrorLogFileName(THROTTLE_CONTROL_ERROR_LOGFILE);

    while(1){
        receiveCommandFromEcu(&cmd);
        if (isThrottleFailed()) handleFailedThrottleCommand(cmd);
        else handleSucceedThrottleCommand(cmd);
    }
}



bool isThrottleFailed() {
    return randDouble(0, 1)<=FAIL_PROBABILITY;
}

void receiveCommandFromEcu(ThrottleControlCommand *pCommand) {
//TODO: Implement receiveCommandFromEcu
}

void handleSucceedThrottleCommand(ThrottleControlCommand command) {
    char message[32];
    char *commandType;

    switch (command.type) {

        case Increment:
            commandType = INCREMENT;
            break;
    }

    sprintf(message, "%s %s %lu", getCurrentDateTime(), commandType, command.quantity);
    logMessage(message);
}

void handleFailedThrottleCommand(ThrottleControlCommand command) {
    signalAboutFailedThrottleCommandToEcu();
}

void signalAboutFailedThrottleCommandToEcu() {
//TODO: Implement signalAboutFailedThrottleCommandToEcu
}
