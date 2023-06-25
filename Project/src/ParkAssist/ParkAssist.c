#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include "ParkAssistIpc.h"
#include "../Logger/Logger.h"
#include "../FilePathProvider/FilePathProvider.h"
#include "../Shared/Consts.h"
#include "../InterProcessComunication/Ipc.h"
#include "../CentralEcu/CentralEcuIpc.h"
#include "../Shared/Utils.h"
#include "ParkAssist.h"


char dataSourceFilePath[128];
int dataSourceFileFd;

int paSocketFd;
int acceptedSocketFd;

int surroundViewCamerasPid;

char executionType[3];

void handleInterruptSignal();

int receiveParkCommandFromEcu();

int receive8BytesFromSurroundViewCameras(char buffer[8]);

int readBytes(char *buffer, unsigned int nBytes);

void closeFileDescriptors();


void runSurroundViewCameras();

void stopSurroundViewCameras();

void registerSignalHandlers();

void instantiatePaSocket();

void assignDataSourceFilePath(char *argStr);

void logBytes(const char *bytes, unsigned int nBytes);

int main(int argc, char *argv[]) {

    if (argc <= 1) {
        logErrorMessage("Unassigned execution type argument");
        closeFileDescriptors();
        return -1;
    }

    strcpy(executionType, argv[1]);

    assignDataSourceFilePath(argv[1]);

    if (strlen(dataSourceFilePath) == 0) {
        logErrorMessage("Incorrect execution type argument");
        closeFileDescriptors();
        exit(-1);
    }


    registerSignalHandlers();

    setLogFileName(PARK_ASSIST_LOGFILE);
    setErrorLogFileName(PARK_ASSIST_ERROR_LOGFILE);
    instantiateLogFileDescriptor();
    instantiateErrorLogFileDescriptor();


    dataSourceFileFd = open(dataSourceFilePath, O_RDONLY);
    if (dataSourceFileFd < 0) {
        logMessage(dataSourceFilePath);
        logLastErrorWithWhenMessage("Data source file not found.");
        closeFileDescriptors();
        return -1;
    }

    instantiatePaSocket();


    char buffer[8];
    while (1) {
        if (receiveParkCommandFromEcu() < 0) continue;
        runSurroundViewCameras();
        for (int i = 0; i < 30; ++i) {
            if (receive8BytesFromSurroundViewCameras(buffer) >= 0) {
                sendDataToEcu(ParkAssistToCentralEcuRequester, buffer, 8);
            }
            if (readBytes(buffer, 8) == 8) {
                sendDataToEcu(ParkAssistToCentralEcuRequester, buffer, 8);
                logBytes(buffer, 8);
            }
            sleep(1);
        }
        stopSurroundViewCameras();
    }
}

void assignDataSourceFilePath(char *argStr) {
    if (strcmp(argStr, NORMAL_EXECUTION) == 0)
        strcpy(dataSourceFilePath, NORMAL_EXECUTION_RANDOM_DATASOURCE);
    if (strcmp(argStr, ARTIFICIAL_EXECUTION) == 0)
        getCwdWithFileName(ARTIFICIAL_EXECUTION_RANDOM_DATASOURCE, dataSourceFilePath, sizeof(dataSourceFilePath));
}

void instantiatePaSocket() {
    paSocketFd = createInetSocket(DEFAULT_PROTOCOL);
    if (paSocketFd < 0) {
        logLastErrorWithWhenMessage("creating a socket for the pa");
        closeFileDescriptors();
        exit(-1);
    }

    if (bindLocalInetSocket(paSocketFd, PARK_ASSIST_INET_SOCKET_PORT) < 0) {
        logLastErrorWithWhenMessage("binding a socket for the pa");
        closeFileDescriptors();
        exit(-1);
    }
    if (listenSocket(paSocketFd, 5) < 0) {
        logLastErrorWithWhenMessage("listening a socket for the pa");
        closeFileDescriptors();
        exit(-1);
    }

    fcntl(paSocketFd, F_SETFL, fcntl(paSocketFd, F_GETFL) | O_NONBLOCK);
}

void registerSignalHandlers() {
    signal(SIGCHLD, SIG_IGN);
    signal(SIGINT, handleInterruptSignal);
}

void handleInterruptSignal() {
    closeFileDescriptors();
    stopSurroundViewCameras();
    exit(0);
}


void runSurroundViewCameras() {
    surroundViewCamerasPid = fork();
    if (surroundViewCamerasPid == 0) {
        closeFileDescriptors();
        execEcuChildProcessWithArgument(SURROUND_VIEW_CAMERAS_EXE_FILENAME, executionType);
    }
}

void stopSurroundViewCameras() {
    if (surroundViewCamerasPid != 0) kill(surroundViewCamerasPid, SIGINT);
    surroundViewCamerasPid = 0;
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
        logLastErrorWithWhenMessage("reading the request sent to the pa from surround view cameras");
        closeSocket(acceptedSocketFd);
        return -1;
    }

    if (requesterId != SurroundViewCamerasToParkAssistRequester) {
        logErrorMessage("Incorrect requester. Expected surround view cameras to be the requester");
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
        logLastErrorWithWhenMessage("reading the request sent to the pa from centralEcu");
        closeSocket(acceptedSocketFd);
        return -1;
    }

    free(requestData);
    closeSocket(acceptedSocketFd);

    if (requesterId != CentralEcuToParkAssistRequester) {
        logErrorMessage("Incorrect requester. Expected central ecu to be the requester");
        return -1;
    }
    return 0;
}

void logBytes(const char *bytes, unsigned int nBytes) {
    char logString[128];
    memset(logString, 0, sizeof(logString));
    convertBytesToStringRepresentation(logString, bytes, nBytes);
    logMessage(logString);
}
