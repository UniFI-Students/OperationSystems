#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include "SurroundViewCameras.h"
#include "../Logger/Logger.h"
#include "../Shared/Consts.h"
#include "../FilePathProvider/FilePathProvider.h"
#include "../InterProcessComunication/Ipc.h"
#include "../ParkAssist/ParkAssistIpc.h"
#include "../Shared/Utils.h"



char dataSourceFilePath[128];
int dataSourceFileFd;

int readBytes(char *buffer, unsigned int nBytes);

void sendBytesToParkAssist(const char *bytes, unsigned int nBytes);

void closeFileDescriptors();

void handleInterruptSignal();

void assignDataSourceFilePath(char *argStr);

void logBytes(const char *bytes, unsigned int nBytes);

int main(int argc, char *argv[]) {

    if (argc <= 1) {
        logErrorMessage("Unassigned execution type argument");
        closeFileDescriptors();
        return -1;
    }

    assignDataSourceFilePath(argv[1]);

    if (strlen(dataSourceFilePath) == 0) {
        logErrorMessage("Incorrect execution type argument");
        closeFileDescriptors();
        exit(-1);
    }

    signal(SIGINT, handleInterruptSignal);


    setLogFileName(SURROUND_CAMERAS_LOGFILE);
    setErrorLogFileName(SURROUND_CAMERAS_ERROR_LOGFILE);
    instantiateLogFileDescriptor();
    instantiateErrorLogFileDescriptor();

    dataSourceFileFd = open(dataSourceFilePath, O_RDONLY);
    if (dataSourceFileFd < 0) {
        logMessage(dataSourceFilePath);
        logErrorMessage("Data source file not found.");
        closeFileDescriptors();
        return -1;
    }

    setLogFileName(SURROUND_CAMERAS_LOGFILE);
    setErrorLogFileName(SURROUND_CAMERAS_ERROR_LOGFILE);
    instantiateLogFileDescriptor();
    instantiateErrorLogFileDescriptor();

    char buffer[8];
    while (1) {

        if (readBytes(buffer, 8) == 8) {
            sendBytesToParkAssist(buffer, 8);
            logBytes(buffer, 8);
        }
        sleep(1);
    }
}

void logBytes(const char *bytes, unsigned int nBytes) {
    char logString[128];
    memset(logString, 0, sizeof(logString));
    convertBytesToStringRepresentation(logString, bytes, nBytes);
    logMessage(logString);
}

void assignDataSourceFilePath(char *argStr) {
    if (strcmp(argStr, NORMAL_EXECUTION) == 0)
        strcpy(dataSourceFilePath, NORMAL_EXECUTION_RANDOM_DATASOURCE);
    if (strcmp(argStr, ARTIFICIAL_EXECUTION) == 0)
        getCwdWithFileName(ARTIFICIAL_EXECUTION_RANDOM_DATASOURCE, dataSourceFilePath, sizeof(dataSourceFilePath));
}

void handleInterruptSignal() {
    closeFileDescriptors();
    exit(0);
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
        logLastErrorWithWhenMessage("writing a request to the pa");
        closeSocket(paSocketFd);
        return;
    }
    if (closeSocket(paSocketFd) < 0) logLastError();
}

int readBytes(char *buffer, unsigned int nBytes) {
    return (int) read(dataSourceFileFd, buffer, nBytes);
}