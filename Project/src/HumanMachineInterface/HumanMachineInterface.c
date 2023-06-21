#include <stdio.h>
#include <string.h>
#include "HumanMachineInterface.h"
#include "HumanMachineInterfaceIpc.h"
#include "../InterProcessComunication/Ipc.h"
#include "../CentralEcu/CentralEcuIpc.h"

#define START "INIZIO"
#define PARKING "PARCHEGGIO"
#define STOP "ARRESTO"

void executeHmiReader();
void executeHmiWriter();

void sendCommandToEcu(int socketFd, HumanMachineInterfaceCommand command);
void receiveMessageFromEcu(char* message);


int main(int argc, char *argv[]) {
    if (argc <= 1) {
        perror("Unassigned argument for running topology.");
        return -1;
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

    int ecuSocketFd = createUnixSocket(DEFAULT_PROTOCOL);

    while (1) {
        scanf("%s", buff);

        if (strcmp(START, buff) == 0) cmd.type = Start;
        if (strcmp(PARKING, buff) == 0) cmd.type = Parking;
        if (strcmp(STOP, buff) == 0) cmd.type = Stop;

        sendCommandToEcu(ecuSocketFd, cmd);
    }
}

void executeHmiWriter() {
    while (1) {
        char message[32];
        receiveMessageFromEcu(message);
        printf("%s\n", message);
    }
}


void sendCommandToEcu(int ecuSocketFd, HumanMachineInterfaceCommand command) {
    writeRequest(ecuSocketFd, HumanMachineInterface, &command, sizeof(command));
}

void receiveMessageFromEcu(char* message) {
    //TODO: Implement receiveMessageFromEcu
}




