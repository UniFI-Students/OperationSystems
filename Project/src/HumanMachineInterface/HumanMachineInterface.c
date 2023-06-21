#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "HumanMachineInterface.h"
#include "HumanMachineInterfaceIpc.h"
#include "../Logger/Logger.h"
#include "../InterProcessComunication/Ipc.h"
#include "../CentralEcu/CentralEcuIpc.h"

#define START "INIZIO"
#define PARKING "PARCHEGGIO"
#define STOP "ARRESTO"

void executeHmiReader();

void executeHmiWriter();

void sendCommandToEcu(HumanMachineInterfaceCommand command);

void receiveMessageFromEcu(char *message);


int main(int argc, char *argv[]) {
    if (argc <= 1) {
        logError("Unassigned argument for running topology.");
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
    char buff[32];
    HumanMachineInterfaceCommand cmd;


    while (1) {
        scanf("%s", buff);

        if (strcmp(START, buff) == 0) cmd.type = Start;
        if (strcmp(PARKING, buff) == 0) cmd.type = Parking;
        if (strcmp(STOP, buff) == 0) cmd.type = Stop;

        sendCommandToEcu(cmd);
    }
}

int hmiSocketFd;


void executeHmiWriter() {
    hmiSocketFd = createInetSocket(DEFAULT_PROTOCOL);
    if (hmiSocketFd < 0) {
        logLastError();
        return;
    }
    if (connectLocalInetSocket(hmiSocketFd, HUMAN_MACHINE_INTERFACE_INET_SOCKET_PORT) < 0) {
        logLastError();
        closeSocket(hmiSocketFd);
        return;
    }

    while (1) {
        char message[32];
        receiveMessageFromEcu(message);
        printf("%s\n", message);
    }
}


void sendCommandToEcu(HumanMachineInterfaceCommand command) {
    int ecuSocketFd = createInetSocket(DEFAULT_PROTOCOL);
    if (ecuSocketFd < 0) {
        logLastError();
        return;
    }
    if (connectLocalInetSocket(ecuSocketFd, CENTRAL_ECU_INET_SOCKET_PORT) < 0) {
        logLastError();
        closeSocket(ecuSocketFd);
        return;
    }
    if (writeRequest(ecuSocketFd, HumanMachineInterfaceToCentralEcuRequester, &command, sizeof(command)) < 0) {
        logLastError();
        exit(-1);
    }
    if (closeSocket(ecuSocketFd) < 0) logLastError();
}

void receiveMessageFromEcu(char *message) {
    int requesterId;
    void *requestData;
    int requestDataLength;

    if (readRequest(hmiSocketFd, &requesterId, &requestData, &requestDataLength) < 0) logLastError();
    switch ((HmiRequester) requesterId) {
        case CentralEcuToHmiRequester:
            strcpy(message, requestData);
            break;
        default:
            logError("Unknown request arrived.");
            break;
    }
    free(requestData);
}




