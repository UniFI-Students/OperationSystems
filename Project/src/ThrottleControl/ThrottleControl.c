#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include "ThrottleControlIpc.h"
#include "../Logger/Logger.h"
#include "../Random/Random.h"
#include "../DateProvider/DateProvider.h"
#include "../Shared/Consts.h"
#include "../InterProcessComunication/Ipc.h"
#include "ThrottleControl.h"


int cEcuPid;

int tcSocketFd;
int acceptedSocketFd;

void receiveCommandFromEcu(ThrottleControlCommand *pCommand);

void handleSucceedThrottleCommand(ThrottleControlCommand command);

bool isThrottleFailed();

void handleFailedThrottleCommand(ThrottleControlCommand command);

void signalAboutFailedThrottleCommandToEcu();

void closeFileDescriptors();

void handleInterruptSignal();

void instantiateTcSocket();

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        logLastErrorWithWhenMessage("Unassigned argument for central ecu pid.");
        exit(-1);
    }
    cEcuPid = atoi(argv[1]);

    signal(SIGINT, handleInterruptSignal);

    startRand(time(NULL));

    ThrottleControlCommand cmd;

    setLogFileName(THROTTLE_CONTROL_LOGFILE);
    setErrorLogFileName(THROTTLE_CONTROL_ERROR_LOGFILE);
    instantiateLogFileDescriptor();
    instantiateErrorLogFileDescriptor();

    instantiateTcSocket();


    while (1) {
        receiveCommandFromEcu(&cmd);
        if (isThrottleFailed()) handleFailedThrottleCommand(cmd);
        else handleSucceedThrottleCommand(cmd);
    }
}

void instantiateTcSocket() {
    tcSocketFd = createInetSocket(DEFAULT_PROTOCOL);
    if (tcSocketFd < 0) {
        logLastErrorWithWhenMessage("creating a socket for the tc");
        closeFileDescriptors();
        exit(-1);
    }

    if (bindLocalInetSocket(tcSocketFd, THROTTLE_CONTROL_INET_SOCKET_PORT) < 0) {
        logLastErrorWithWhenMessage("binding a socket for the tc");
        closeFileDescriptors();
        exit(-1);
    }
    if (listenSocket(tcSocketFd, 5) < 0) {
        logLastErrorWithWhenMessage("listening a socket for the tc");
        closeFileDescriptors();
        exit(-1);
    }
}

void handleInterruptSignal() {
    closeFileDescriptors();
    exit(0);
}

void closeFileDescriptors() {
    closeSocket(tcSocketFd);
    closeSocket(acceptedSocketFd);
    closeLogFileDescriptor();
    closeErrorLogFileDescriptor();
}


bool isThrottleFailed() {
    if (randDouble(0, 1) < FAIL_PROBABILITY) return true;
    return false;
}

void receiveCommandFromEcu(ThrottleControlCommand *pCommand) {
    acceptedSocketFd = acceptInetSocket(tcSocketFd);
    if (acceptedSocketFd < 0) {
        logLastErrorWithWhenMessage("accepting a request to the tc");
        return;
    }

    int requesterId;
    void *requestData;
    unsigned int requestDataLength;

    if (readRequest(acceptedSocketFd, &requesterId, &requestData, &requestDataLength) < 0) {
        logLastErrorWithWhenMessage("reading the request sent to the tc");
        closeSocket(acceptedSocketFd);
        return;
    }

    ThrottleControlCommand *cmdPtr = (ThrottleControlCommand *) requestData;

    switch ((ThrottleControlRequester) requesterId) {

        case CentralEcuToTcRequester:
            pCommand->type = cmdPtr->type;
            pCommand->quantity = cmdPtr->quantity;
            break;
        default:
            logErrorMessage("Unknown request arrived.");
            break;
    }


    free(requestData);
    closeSocket(acceptedSocketFd);
}

void handleSucceedThrottleCommand(ThrottleControlCommand command) {
    char message[32];
    char *commandType;

    switch (command.type) {

        case Increment:
            commandType = INCREMENT;
            break;
        default:
            logErrorMessage("Unhandled throttle command because of undeclared command type");
            return;
    }

    sprintf(message, "%s %s %lu", getCurrentDateTime(), commandType, command.quantity);
    logMessage(message);
}

void handleFailedThrottleCommand(ThrottleControlCommand command) {
    signalAboutFailedThrottleCommandToEcu();
}

void signalAboutFailedThrottleCommandToEcu() {
    kill(cEcuPid, SIG_THROTTLE_FAIL);
}
