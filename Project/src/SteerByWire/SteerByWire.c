#include <unistd.h>
#include "SteerByWireIpc.h"
#include "../Logger/Logger.h"
#include "../InterProcessComunication/Ipc.h"
#include "../Shared/Consts.h"
#include "SteerByWire.h"
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>



int sbwSocketFd;
int acceptedSocketFd;

bool receiveCommandFromEcu(SteerByWireCommand *pCommand);

void handleSteerRightCommand();

void handleNoAction();

void handleSteerLeftCommand();


void closeFileDescriptors();

void handleInterruptSignal();

void instantiateSbwSocket();

int main(void) {
    SteerByWireCommand steerCommand;

    setLogFileName(STEER_BY_WIRE_LOGFILE);
    setErrorLogFileName(STEER_BY_WIRE_ERROR_LOGFILE);
    instantiateLogFileDescriptor();
    instantiateErrorLogFileDescriptor();

    instantiateSbwSocket();

    signal(SIGINT, handleInterruptSignal);

    while (1) {
        if (receiveCommandFromEcu(&steerCommand)) {

            switch (steerCommand.type) {
                case Left:
                    handleSteerLeftCommand();
                    break;
                case Right:
                    handleSteerRightCommand();
                    break;
            }
        } else {
            handleNoAction();
            sleep(1);
        }
    }
}

void instantiateSbwSocket() {
    sbwSocketFd = createInetSocket(DEFAULT_PROTOCOL);
    if (sbwSocketFd < 0) {
        logLastErrorWithWhenMessage("creating a socket for the sbw");
        closeFileDescriptors();
        exit(-1);
    }

    fcntl(sbwSocketFd, F_SETFL, fcntl(sbwSocketFd, F_GETFL, 0) | O_NONBLOCK);

    if (bindLocalInetSocket(sbwSocketFd, STEER_BY_WIRE_INET_SOCKET_PORT) < 0) {
        logLastErrorWithWhenMessage("binding a socket for the sbw");
        closeFileDescriptors();
        exit(-1);
    }
    if (listenSocket(sbwSocketFd, 5) < 0) {
        logLastErrorWithWhenMessage("listening a socket for the sbw");
        closeFileDescriptors();
        exit(-1);
    }
}

void handleInterruptSignal() {
    closeFileDescriptors();
    exit(0);

}

void closeFileDescriptors() {
    closeSocket(acceptedSocketFd);
    closeSocket(sbwSocketFd);
    closeLogFileDescriptor();
    closeErrorLogFileDescriptor();

}

void handleSteerLeftCommand() {
    for (int i = 0; i < 4; ++i) {
        logMessage(STEERING_LEFT);
        sleep(1);
    }
}


void handleNoAction() {
    logMessage(NO_ACTION);
}

void handleSteerRightCommand() {
    for (int i = 0; i < 4; ++i) {
        logMessage(STEERING_RIGHT);
        sleep(1);
    }
}

bool receiveCommandFromEcu(SteerByWireCommand *pCommand) {
    acceptedSocketFd = acceptInetSocket(sbwSocketFd);
    if (acceptedSocketFd < 0){
        errno = 0;
        return false;
    }

    int requesterId;
    void *requestData;
    unsigned int requestDataLength;

    if (readRequest(acceptedSocketFd, &requesterId, &requestData, &requestDataLength) < 0){
        logLastErrorWithWhenMessage("reading the request send to the sbw");
        closeSocket(acceptedSocketFd);
        return false;
    }

    SteerByWireCommand *cmdPtr = (SteerByWireCommand *) requestData;

    switch ((SteerByWireRequester)requesterId) {

        case CentralEcuToSbwRequester:
            pCommand->type = cmdPtr->type;
            break;
        default:
            logErrorMessage("Unknown request arrived.");
            break;
    }

    free(requestData);
    closeSocket(acceptedSocketFd);
    return true;
}
