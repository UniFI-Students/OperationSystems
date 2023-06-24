#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "HumanMachineInterfaceIpc.h"
#include "../Logger/Logger.h"
#include "../InterProcessComunication/Ipc.h"
#include "../CentralEcu/CentralEcuIpc.h"
#include "../Shared/Consts.h"

#define START "INIZIO"
#define PARKING "PARCHEGGIO"
#define STOP "ARRESTO"

#define HUMAN_MACHINE_INTERFACE_ERROR_LOGFILE "hmi.eLog"


int hmiSocketFd;
int acceptedSocketFd;

void executeHmiReader();

void executeHmiWriter();

void sendCommandToEcu(HumanMachineInterfaceCommand command);

void receiveMessageFromEcu(char *message);


void closeFileDescriptors();

int main(int argc, char *argv[]) {
    setErrorLogFileName(HUMAN_MACHINE_INTERFACE_ERROR_LOGFILE);
    instantiateErrorLogFileDescriptor();

    if (argc <= 1) {
        logLastErrorWithMessage("Unassigned argument for running topology.");
        exit(-1);
    }

    if (strcmp(argv[1], "-w") == 0) {
        executeHmiWriter();
    }
    if (strcmp(argv[1], "-r") == 0) {
        executeHmiReader();
    }

    return 0;
}

void executeHmiReader() {
    HumanMachineInterfaceCommand cmd;
    char buff[32];
    while (1) {
        memset(buff, 0, sizeof(buff));
        cmd.type = None;
        scanf("%s", buff);

        if (strcmp(START, buff) == 0) cmd.type = Start;
        if (strcmp(PARKING, buff) == 0) cmd.type = Parking;
        if (strcmp(STOP, buff) == 0) cmd.type = Stop;

        if (cmd.type != None) sendCommandToEcu(cmd);
    }
}


void executeHmiWriter() {
    hmiSocketFd = createInetSocket(DEFAULT_PROTOCOL);
    if (hmiSocketFd < 0) {
        logLastError();
        closeFileDescriptors();
        exit(-1);
    }

    if (bindLocalInetSocket(hmiSocketFd, HUMAN_MACHINE_INTERFACE_INET_SOCKET_PORT) < 0) {
        logLastError();
        closeFileDescriptors();
        exit(-1);
    }
    if (listenSocket(hmiSocketFd, 5) < 0) {
        logLastError();
        closeFileDescriptors();
        exit(-1);
    }

    while (1) {
        char message[32];
        receiveMessageFromEcu(message);
        printf("%s\n", message);
    }
}

void closeFileDescriptors() {
    closeSocket(hmiSocketFd);
    closeSocket(acceptedSocketFd);
    closeErrorLogFileDescriptor();
}


void sendCommandToEcu(HumanMachineInterfaceCommand command) {
    int ecuSocketFd = createInetSocket(DEFAULT_PROTOCOL);
    if (ecuSocketFd < 0) {
        logLastError();
        return;
    }
    if (connectLocalInetSocket(ecuSocketFd, CENTRAL_ECU_INET_SOCKET_PORT) < 0) {
        logLastErrorWithMessage("Could not establish connection to CentralEcu");
        closeSocket(ecuSocketFd);
        return;
    }
    if (writeRequest(ecuSocketFd, HumanMachineInterfaceToCentralEcuRequester, &command, sizeof(command)) < 0) {
        logLastError();
        closeSocket(ecuSocketFd);
        return;
    }
    if (closeSocket(ecuSocketFd) < 0) logLastError();
}

void receiveMessageFromEcu(char *message) {
    acceptedSocketFd = acceptInetSocket(hmiSocketFd);

    if (acceptedSocketFd < 0){
        logLastError();
        return;
    }

    int requesterId;
    void *requestData;
    unsigned int requestDataLength;

    if (readRequest(acceptedSocketFd, &requesterId, &requestData, &requestDataLength) < 0) {
        logLastError();
        closeSocket(acceptedSocketFd);
        return;
    }
    switch ((HmiRequester) requesterId) {
        case CentralEcuToHmiRequester:
            strcpy(message, requestData);
            break;
        default:
            logLastErrorWithMessage("Unknown request arrived.");
            break;
    }
    free(requestData);
    closeSocket(acceptedSocketFd);
}




