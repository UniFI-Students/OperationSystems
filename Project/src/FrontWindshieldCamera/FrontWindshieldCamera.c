#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include "FrontWindshieldCamera.h"
#include "../Logger/Logger.h"
#include "../InterProcessComunication/Ipc.h"
#include "../Shared/Consts.h"
#include "../CentralEcu/CentralEcuIpc.h"
#include "../FilePathProvider/FilePathProvider.h"

#define FRONT_CAMERA_DATA_SOURCE_FILE "frontCamera.data"
#define FRONT_CAMERA_LOGFILE "camera.log"
#define FRONT_CAMERA_ERROR_LOGFILE "camera.eLog"

FILE* dataSourceFilePtr;
char dataSourceFilePath[128];

void readDataFromSourceFile(char *dataBuff);
void sendDataToEcu(const char* message);
void closeFileDescriptors();

void handleInterruptSignal();

int main()
{
    signal(SIGINT, handleInterruptSignal);
    char buff[64];
    getCwdWithFileName(FRONT_CAMERA_DATA_SOURCE_FILE, dataSourceFilePath, sizeof(dataSourceFilePath));

    setLogFileName(FRONT_CAMERA_LOGFILE);
    setErrorLogFileName(FRONT_CAMERA_ERROR_LOGFILE);

    instantiateLogFileDescriptor();
    instantiateErrorLogFileDescriptor();
    dataSourceFilePtr = fopen(dataSourceFilePath, "r");

    while(!feof(dataSourceFilePtr))
    {
        readDataFromSourceFile(buff);
        sendDataToEcu(buff);
        logMessage(buff);
        sleep(1);
    }
    closeFileDescriptors();
}

void handleInterruptSignal() {
    closeFileDescriptors();
    exit(0);
}

void closeFileDescriptors() {
    fclose(dataSourceFilePtr);
    closeLogFileDescriptor();
    closeErrorLogFileDescriptor();
}

void sendDataToEcu(const char* message) {
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
    if (writeRequest(ecuSocketFd, FrontWindShieldCameraToCentralEcuRequester, message, strlen(message)) < 0) {
        logLastError();
        closeSocket(ecuSocketFd);
        return;
    }
    if (closeSocket(ecuSocketFd) < 0) logLastError();
}

void readDataFromSourceFile(char *dataBuff)
{
    fscanf(dataSourceFilePtr, "%s\n", dataBuff);
}
