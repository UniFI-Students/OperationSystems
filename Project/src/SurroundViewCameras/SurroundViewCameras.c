#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include "SurroundViewCameras.h"
#include "../Logger/Logger.h"
#include "../Shared/Consts.h"
#include "../FilePathProvider/FilePathProvider.h"
#include "../InterProcessComunication/Ipc.h"
#include "../ParkAssist/ParkAssistIpc.h"


#define SURROUND_CAMERAS_LOGFILE "cameras.log"
#define SURROUND_CAMERAS_ERROR_LOGFILE "cameras.ELog"

char dataSourceFilePath[128];
int dataSourceFileFd;

int readBytes(char *buffer, unsigned int nBytes);

void sendBytesToParkAssist(const char *bytes, unsigned int nBytes);

void closeFileDescriptors();

void convertToStringRepresentation(char *dest, const char *source, unsigned int size);

void handleInterruptSignal();

int main(int argc, char *argv[]) {
    signal(SIGINT, handleInterruptSignal);
    char buffer[8];

    setLogFileName(SURROUND_CAMERAS_LOGFILE);
    setErrorLogFileName(SURROUND_CAMERAS_ERROR_LOGFILE);
    instantiateLogFileDescriptor();
    instantiateErrorLogFileDescriptor();

    if (argc <= 1) {
        logLastErrorWithWhenMessage("Unassigned execution type argument.");
        closeFileDescriptors();
        return -1;
    }

    if (strcmp(argv[1], NORMAL_EXECUTION) == 0)
        strcpy(dataSourceFilePath, NORMAL_EXECUTION_RANDOM_DATASOURCE);
    if (strcmp(argv[1], ARTIFICIAL_EXECUTION) == 0)
        getCwdWithFileName(ARTIFICIAL_EXECUTION_RANDOM_DATASOURCE, dataSourceFilePath, sizeof(dataSourceFilePath));

    if (strlen(dataSourceFilePath) == 0) {
        logLastErrorWithWhenMessage("Incorrect execution type argument.");
        closeFileDescriptors();
        return -1;
    }

    dataSourceFileFd = open(dataSourceFilePath, O_RDONLY);
    if (dataSourceFileFd < 0) {
        logMessage(dataSourceFilePath);
        logLastErrorWithWhenMessage("Data source file not found.");
        closeFileDescriptors();
        return -1;
    }

    setLogFileName(SURROUND_CAMERAS_LOGFILE);
    setErrorLogFileName(SURROUND_CAMERAS_ERROR_LOGFILE);
    instantiateLogFileDescriptor();
    instantiateErrorLogFileDescriptor();

    strcpy(dataSourceFilePath, argv[1]);

    while (1) {

        if (readBytes(buffer, 8) == 8) {
            char logString[128];
            memset(logString, 0, sizeof(logString));
            sendBytesToParkAssist(buffer, 8);
            convertToStringRepresentation(logString, buffer, 8);
            logMessage(logString);
        }
        sleep(1);
    }
}

void handleInterruptSignal() {
    closeFileDescriptors();
    exit(0);
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

void sendBytesToParkAssist(const char *bytes, unsigned int nBytes) {
    int paSocketFd = createInetSocket(DEFAULT_PROTOCOL);
    if (paSocketFd < 0) {
        logLastError();
        return;
    }
    if (connectLocalInetSocket(paSocketFd, CENTRAL_ECU_INET_SOCKET_PORT) < 0) {
        logLastErrorWithWhenMessage("Could not establish connection to Park assist");
        closeSocket(paSocketFd);
        return;
    }
    if (writeRequest(paSocketFd, SurroundViewCamerasToParkAssistRequester, bytes, nBytes) < 0) {
        logLastError();
        closeSocket(paSocketFd);
        return;
    }
    if (closeSocket(paSocketFd) < 0) logLastError();
}

int readBytes(char *buffer, unsigned int nBytes) {
    return (int) read(dataSourceFileFd, buffer, nBytes);
}