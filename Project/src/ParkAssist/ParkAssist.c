#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include "ParkAssistIpc.h"
#include "../Logger/Logger.h"
#include "../FilePathProvider/FilePathProvider.h"
#include "../Shared/Consts.h"
#include "../InterProcessComunication/Ipc.h"
#include "../CentralEcu/CentralEcuIpc.h"

#define PARK_ASSIST_LOGFILE "assist.log"
#define PARK_ASSIST_ERROR_LOGFILE "assist.eLog"

char dataSourceFilePath[128];
int dataSourceFileFd;

int paSocketFd;
int acceptedSocketFd;

int surroundViewCamerasPid;

char executionType[3];

void handleInterruptSignal();

int receiveParkCommandFromEcu();

int receive8BytesFromSurroundViewCameras(char buffer[8]);

void sendBytesToEcu(const char *bytes, unsigned int nBytes);

int readBytes(char *buffer, unsigned int nBytes);

void closeFileDescriptors();

void convertToStringRepresentation(char *dest, const char *source, unsigned int size);

void execEcuChildProcessWithArgument(const char *childName, const char *arg);

void runSurroundViewCameras();

void stopSurroundViewCameras();

void registerSignalHandlers();

int main(int argc, char *argv[]) {
    char buffer[8];

    registerSignalHandlers();

    setLogFileName(PARK_ASSIST_LOGFILE);
    setErrorLogFileName(PARK_ASSIST_ERROR_LOGFILE);
    instantiateLogFileDescriptor();
    instantiateErrorLogFileDescriptor();

    if (argc <= 1) {
        logLastErrorWithMessage("Unassigned execution type argument.");
        closeFileDescriptors();
        return -1;
    }

    if (strcmp(argv[1], NORMAL_EXECUTION) == 0)
        strcpy(dataSourceFilePath, NORMAL_EXECUTION_RANDOM_DATASOURCE);
    if (strcmp(argv[1], ARTIFICIAL_EXECUTION) == 0)
        getCwdWithFileName(ARTIFICIAL_EXECUTION_RANDOM_DATASOURCE, dataSourceFilePath, sizeof(dataSourceFilePath));

    if (strlen(dataSourceFilePath) == 0) {
        logLastErrorWithMessage("Incorrect execution type argument.");
        closeFileDescriptors();
        return -1;
    }

    strcpy(executionType, argv[1]);

    dataSourceFileFd = open(dataSourceFilePath, O_RDONLY);
    if (dataSourceFileFd < 0) {
        logMessage(dataSourceFilePath);
        logLastErrorWithMessage("Data source file not found.");
        closeFileDescriptors();
        return -1;
    }

    paSocketFd = createInetSocket(DEFAULT_PROTOCOL);
    if (paSocketFd < 0) {
        logLastError();
        closeFileDescriptors();
        exit(-1);
    }

    if (bindLocalInetSocket(paSocketFd, PARK_ASSIST_INET_SOCKET_PORT) < 0) {
        logLastError();
        closeFileDescriptors();
        exit(-1);
    }
    if (listenSocket(paSocketFd, 5) < 0) {
        logLastError();
        closeFileDescriptors();
        exit(-1);
    }

    fcntl(paSocketFd, F_SETFL, fcntl(paSocketFd, F_GETFL) | O_NONBLOCK);


    while (1) {
        if (receiveParkCommandFromEcu() < 0) continue;
        runSurroundViewCameras();
        for (int i = 0; i < 30; ++i) {
            if (receive8BytesFromSurroundViewCameras(buffer) >= 0) {
                sendBytesToEcu(buffer, 8);
            }
            if (readBytes(buffer, 8) == 8) {
                char logString[128];
                memset(logString, 0, sizeof(logString));
                sendBytesToEcu(buffer, 8);
                convertToStringRepresentation(logString, buffer, 8);
                logMessage(logString);
            }
            sleep(1);
        }
        stopSurroundViewCameras();
    }
}

void registerSignalHandlers() {
    signal(SIGCHLD, SIG_IGN);
    signal(SIGINT, handleInterruptSignal);
}

void handleInterruptSignal()
{
    closeFileDescriptors();
    stopSurroundViewCameras();
    exit(0);
}


void runSurroundViewCameras() {
    surroundViewCamerasPid = fork();
    if (surroundViewCamerasPid == 0) execEcuChildProcessWithArgument(SURROUND_VIEW_CAMERAS_EXE_FILENAME, executionType);
}

void stopSurroundViewCameras() {
    if (surroundViewCamerasPid != 0) kill(surroundViewCamerasPid, SIGINT);
    surroundViewCamerasPid = 0;
}

void execEcuChildProcessWithArgument(const char *childName, const char *arg) {
    closeFileDescriptors();
    char buff[128];
    getCwdWithFileName(childName, buff, sizeof(buff));
    execl(buff, childName, arg, (char *) 0);
    logLastError();
    exit(-1);
}

void sendBytesToEcu(const char *bytes, unsigned int nBytes) {
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
    if (writeRequest(ecuSocketFd, ParkAssistToCentralEcuRequester, bytes, nBytes) < 0) {
        logLastError();
        closeSocket(ecuSocketFd);
        return;
    }
    if (closeSocket(ecuSocketFd) < 0) logLastError();
}

void convertToStringRepresentation(char *dest, const char *source, unsigned int size) {
    char convertedValueToHexString[16];
    for (int i = 0; i < 8; ++i) {
        sprintf(convertedValueToHexString, "| 0x%.8X ", source[0]);
        strcat(dest, convertedValueToHexString);
    }
    strcat(dest, "|");
}

void closeFileDescriptors() {
    close(dataSourceFileFd);
    closeLogFileDescriptor();
    closeErrorLogFileDescriptor();

}

int readBytes(char *buffer, unsigned int nBytes) {
    return (int) read(dataSourceFileFd, buffer, nBytes);
}

int receive8BytesFromSurroundViewCameras(char buffer[8]) {
    acceptedSocketFd = acceptInetSocket(paSocketFd);
    if (acceptedSocketFd < 0) {
        return -1;
    }

    int requesterId;
    void *requestData;
    unsigned int requestDataLength;

    if (readRequest(acceptedSocketFd, &requesterId, &requestData, &requestDataLength) < 0) {
        logLastError();
        closeSocket(acceptedSocketFd);
        return -1;
    }

    if (requesterId != SurroundViewCamerasToParkAssistRequester) {
        logLastErrorWithMessage("Incorrect requester.");
        free(requestData);
        closeSocket(acceptedSocketFd);
        return -1;
    }

    memcpy(buffer, requestData, 8);

    free(requestData);
    closeSocket(acceptedSocketFd);
    return 0;
}

int receiveParkCommandFromEcu() {
    acceptedSocketFd = acceptInetSocket(paSocketFd);
    if (acceptedSocketFd < 0) {
        return -1;
    }

    int requesterId;
    void *requestData;
    unsigned int requestDataLength;

    if (readRequest(acceptedSocketFd, &requesterId, &requestData, &requestDataLength) < 0) {
        logLastError();
        closeSocket(acceptedSocketFd);
        return -1;
    }

    free(requestData);
    closeSocket(acceptedSocketFd);

    if (requesterId != CentralEcuToParkAssistRequester) {
        logLastErrorWithMessage("Incorrect requester.");
        return -1;
    }
    return 0;
}
