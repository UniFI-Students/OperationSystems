#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include "BrakeByWireIpc.h"
#include "../Logger/Logger.h"
#include "../DateProvider/DateProvider.h"
#include "../InterProcessComunication/Ipc.h"
#include "../CentralEcu/CentralEcuBbwIpc/CentralEcuBbwIpc.h"
#include "../Shared/Consts.h"

#define BRAKE "FRENO"
#define STOP_CAR "ARRESTO AUTO"
#define BREAK_BY_WIRE_LOGFILE "brake.log"
#define BREAK_BY_WIRE_ERROR_LOGFILE "brake.eLog"

int bbwSocketFd;
int acceptedSocketFd;

void handleBrakeCommand(BrakeByWireCommand command);

void receiveCommandFromEcu(BrakeByWireCommand *pCommand);
void receiveStopSignalFromEcu();

void handleStopSignal();

void registerSignalHandlers();
void closeFileDescriptors();

int main(){
    BrakeByWireCommand cmd;
    setLogFileName(BREAK_BY_WIRE_LOGFILE);
    setErrorLogFileName(BREAK_BY_WIRE_ERROR_LOGFILE);
    instantiateLogFileDescriptor();
    instantiateErrorLogFileDescriptor();

    bbwSocketFd = createInetSocket(DEFAULT_PROTOCOL);
    if (bbwSocketFd < 0) {
        logLastError();
        closeFileDescriptors();
        exit(-1);
    }

    if (bindLocalInetSocket(bbwSocketFd, BRAKE_BY_WIRE_INET_SOCKET_PORT) < 0) {
        logLastError();
        closeFileDescriptors();
        exit(-1);
    }
    if (listenSocket(bbwSocketFd, 5) < 0) {
        logLastError();
        closeFileDescriptors();
        exit(-1);
    }

    registerSignalHandlers();
    while(1)
    {
        receiveCommandFromEcu(&cmd);
        handleBrakeCommand(cmd);
    }
}

void registerSignalHandlers() {
    signal(SIG_STOP_CAR, handleStopSignal);
    signal(SIGINT, closeFileDescriptors);
}

void closeFileDescriptors(){
    closeLogFileDescriptor();
    closeErrorLogFileDescriptor();
    closeSocket(bbwSocketFd);
}


void receiveCommandFromEcu(BrakeByWireCommand *pCommand) {
    acceptedSocketFd = acceptInetSocket(bbwSocketFd);
    if (acceptedSocketFd < 0){
        logLastError();
        return;
    }

    int requesterId;
    void *requestData;
    unsigned int requestDataLength;

    if (readRequest(acceptedSocketFd, &requesterId, &requestData, &requestDataLength) < 0){
        logLastError();
        closeSocket(acceptedSocketFd);
        return;
    }

    BrakeByWireCommand *cmdPtr = (BrakeByWireCommand*) requestData;

    switch ((BreakByWireRequester)requesterId) {

        case CentralEcuToBbwRequester:
            pCommand->type = cmdPtr->type;
            pCommand->quantity=cmdPtr->quantity;
            break;
        default:
            logLastErrorWithMessage("Unknown request arrived.");
            break;
    }


    free(requestData);
    closeSocket(acceptedSocketFd);
}

void handleBrakeCommand(BrakeByWireCommand command) {
    char message[32];
    char *commandType;

    switch (command.type) {

        case Decrement:
            commandType = BRAKE;
            break;
        default:
            return;
    }

    sprintf(message, "%s: %s %lu", getCurrentDateTime(), commandType, command.quantity);
    logMessage(message);
}

void handleStopSignal() {
    logMessage(STOP_CAR);
}
