#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "HumanMachineInterface.h"
#include "HumanMachineInterfaceIpc.h"
#include "../Logger/Logger.h"
#include "../InterProcessComunication/Ipc.h"
#include "../CentralEcu/CentralEcuIpc.h"
#include "../Shared/Consts.h"
#include "../Shared/Utils.h"

int cEcuPid;

int hmiSocketFd;
int acceptedSocketFd;

void executeHmiReader(const char *executionType);

void executeHmiWriter();

void receiveMessageFromEcu(char *message);


void closeFileDescriptors();

void handleInterruptSignal();

void stopCentralEcu();

void runCentralEcu(const char *executionType);

int main(int argc, char *argv[]) {
    signal(SIGINT, handleInterruptSignal);
    setErrorLogFileName(HUMAN_MACHINE_INTERFACE_ERROR_LOGFILE);
    instantiateErrorLogFileDescriptor();

    if (argc <= 1) {
        logErrorMessage("Unassigned argument for running topology.");
        exit(-1);
    }

    if (strcmp(argv[1], "-w") == 0) {
        executeHmiWriter();
    }
    executeHmiReader(argv[1]);

    return 0;
}

void handleInterruptSignal() {
    closeFileDescriptors();
    stopCentralEcu();
}


void executeHmiReader(const char *executionType) {
    runCentralEcu(executionType);
    HumanMachineInterfaceCommand cmd;
    char buff[32];
    while (1) {
        memset(buff, 0, sizeof(buff));
        cmd.type = None;
        scanf("%s", buff);

        if (strcmp(START, buff) == 0) cmd.type = Start;
        if (strcmp(PARKING, buff) == 0) cmd.type = Parking;
        if (strcmp(STOP, buff) == 0) cmd.type = Stop;

        if (cmd.type != None)
            sendDataToEcu(HumanMachineInterfaceToCentralEcuRequester, &cmd, sizeof(cmd));
    }
}

void runCentralEcu(const char *executionType) {
    cEcuPid = fork();
    if (cEcuPid == 0) execEcuChildProcessWithArgument(CENTRAL_ECU_EXE_FILENAME, executionType);
}

void stopCentralEcu() {
    if (cEcuPid != 0) kill(cEcuPid, SIGINT);
    exit(0);
}


void executeHmiWriter() {
    hmiSocketFd = createInetSocket(DEFAULT_PROTOCOL);
    if (hmiSocketFd < 0) {
        logLastErrorWithWhenMessage("creating a socket for the hmi");
        closeFileDescriptors();
        exit(-1);
    }

    if (bindLocalInetSocket(hmiSocketFd, HUMAN_MACHINE_INTERFACE_INET_SOCKET_PORT) < 0) {
        logLastErrorWithWhenMessage("binding a socket for the hmi");
        closeFileDescriptors();
        exit(-1);
    }
    if (listenSocket(hmiSocketFd, 5) < 0) {
        logLastErrorWithWhenMessage("listening a socket for the hmi");
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


void receiveMessageFromEcu(char *message) {
    acceptedSocketFd = acceptInetSocket(hmiSocketFd);

    if (acceptedSocketFd < 0) {
        logLastErrorWithWhenMessage("accepting a request to the hmi");
        return;
    }

    int requesterId;
    void *requestData;
    unsigned int requestDataLength;

    if (readRequest(acceptedSocketFd, &requesterId, &requestData, &requestDataLength) < 0) {
        logLastErrorWithWhenMessage("reading the request sent to the hmi");
        closeSocket(acceptedSocketFd);
        return;
    }
    switch ((HmiRequester) requesterId) {
        case CentralEcuToHmiRequester:
            strcpy(message, requestData);
            break;
        default:
            logErrorMessage("Unknown request arrived.");
            break;
    }
    free(requestData);
    closeSocket(acceptedSocketFd);
}




