
#include <unistd.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "CentralEcuIpc.h"
#include "../Logger/Logger.h"
#include "../InterProcessComunication/Ipc.h"
#include "../HumanMachineInterface/HumanMachineInterfaceIpc.h"

#define CENTRAL_ECU_LOGFILE "ECU.log"
#define CENTRAL_ECU_ERROR_LOGFILE "ECU.eLog"

int cEcuSocketFd;
int hmiSocketFd;

int velocity;
bool isStarted;

void handleInterruptSignal();

void handleHmiRequest(const void *requestData, int requestDataLength);

void registerSigIntHandler();

void handlePaRequest(void *pVoid, int length);

void handleFwscRequest(void *pVoid, int length);

void handleFfrRequest(void *pVoid, int length);

void handleStartCommandFromHmi();

void handleParkingCommandFromHmi();

void handleStopCommandFromHmi();

void sendMessageToHmi(char *message);

void closeFileDescriptors();

int main() {
    cEcuSocketFd = createInetSocket(DEFAULT_PROTOCOL);
    velocity = 0;
    isStarted = false;

    setLogFileName(CENTRAL_ECU_LOGFILE);
    setErrorLogFileName(CENTRAL_ECU_ERROR_LOGFILE);
    instantiateLogFileDescriptor();
    instantiateErrorLogFileDescriptor();

    if (cEcuSocketFd < 0) {
        logLastError();
        closeFileDescriptors();
        exit(-1);
    }
    if (bindLocalInetSocket(cEcuSocketFd, CENTRAL_ECU_INET_SOCKET_PORT) < 0) {
        logLastError();
        closeFileDescriptors();
        exit(-1);
    }
    if (listenSocket(cEcuSocketFd, 5) < 0) {
        logLastError();
        closeFileDescriptors();
        exit(-1);
    }

    hmiSocketFd = createInetSocket(DEFAULT_PROTOCOL);
    if (hmiSocketFd < 0) {
        logLastError();
        closeFileDescriptors();
        exit(-1);
    }
    int hmiConnectionRes = connectLocalInetSocket(hmiSocketFd, HUMAN_MACHINE_INTERFACE_INET_SOCKET_PORT);
    while (hmiConnectionRes < 0) {
        logError("Trying to connect to hmi.");
        hmiConnectionRes = connectLocalInetSocket(hmiSocketFd, HUMAN_MACHINE_INTERFACE_INET_SOCKET_PORT);
    }


    registerSigIntHandler();

    while (1) {
        int acceptedSocket = acceptInetSocket(cEcuSocketFd);
        if (acceptedSocket < 0) {
            logLastError();
            closeFileDescriptors();
            exit(-1);
        }

        int requesterId;
        void *requestData;
        int requestDataLength;

        if (readRequest(acceptedSocket, &requesterId, &requestData, &requestDataLength) < 0) logLastError();

        switch ((CentralEcuRequester) requesterId) {
            case HumanMachineInterfaceToCentralEcuRequester:
                handleHmiRequest(requestData, requestDataLength);
                break;
            case FrontWindShieldCameraToCentralEcuRequester:
                handleFwscRequest(requestData, requestDataLength);
                break;
            case ForwardFacingRadarToCentralEcuRequester:
                handleFfrRequest(requestData, requestDataLength);
                break;
            case ParkAssistToCentralEcuRequester:
                handlePaRequest(requestData, requestDataLength);
                break;
            default:
                logError("Unknown request arrived.");
                break;
        }
        free(requestData);
        if (closeSocket(acceptedSocket) < 0) {
            logLastError();
            closeFileDescriptors();
            exit(-1);
        }
    }

}

void closeFileDescriptors() {
    closeSocket(cEcuSocketFd);
    closeSocket(hmiSocketFd);
    closeLogFileDescriptor();
}


void registerSigIntHandler() {
    signal(SIGINT, handleInterruptSignal);
}

void handleInterruptSignal() {
    closeFileDescriptors();
    exit(0);
}

void handleHmiRequest(const void *requestData, int requestDataLength) {
    HumanMachineInterfaceCommand *cmdPtr = (HumanMachineInterfaceCommand *) requestData;
    switch (cmdPtr->type) {

        case Start:
            handleStartCommandFromHmi();
            break;
        case Parking:
            handleParkingCommandFromHmi();
            break;
        case Stop:
            handleStopCommandFromHmi();
            break;
    }

}

void handleStartCommandFromHmi() {
    isStarted = true;
}

void handleParkingCommandFromHmi() {
    //TODO: Implement handleParkingCommandFromHmi
}

void handleStopCommandFromHmi() {
    //TODO: Implement handleStopCommandFromHmi
}

void sendMessageToHmi(char *message) {
    if (writeRequest(hmiSocketFd, CentralEcuToHmiRequester, message, strlen(message)) < 0) {
        logLastError();
    }
}

void handleFwscRequest(void *pVoid, int length) {
    if (!isStarted) return;
    printf("FWSC REQUEST\n");
}

void handleFfrRequest(void *pVoid, int length) {
    if (!isStarted) return;
    printf("FFR REQUEST\n");
}


void handlePaRequest(void *pVoid, int length) {
    if (!isStarted) return;
    printf("Pa REQUEST\n");
}
