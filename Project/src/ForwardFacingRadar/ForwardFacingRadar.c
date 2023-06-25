#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include "ForwardFacingRadar.h"
#include "../Logger/Logger.h"
#include "../Shared/Consts.h"
#include "../FilePathProvider/FilePathProvider.h"
#include "../InterProcessComunication/Ipc.h"
#include "../CentralEcu/CentralEcuIpc.h"

#define FORWARD_RADAR_LOGFILE "radar.log"
#define FORWARD_RADAR_ERROR_LOGFILE "radar.eLog"


char dataSourceFilePath[128];
int dataSourceFileFd;

int readBytes(char * buffer, unsigned int nBytes);

void sendBytesToEcu(const char *bytes, unsigned int nBytes);


void closeFileDescriptors();

void convertToStringRepresentation(char *dest, const char *source, unsigned int size);

int main(int argc, char *argv[]) {

    char buffer[8];

    setLogFileName(FORWARD_RADAR_LOGFILE);
    setErrorLogFileName(FORWARD_RADAR_ERROR_LOGFILE);
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

    dataSourceFileFd = open(dataSourceFilePath, O_RDONLY);
    if (dataSourceFileFd < 0){
        logMessage(dataSourceFilePath);
        logLastErrorWithMessage("Data source file not found.");
        closeFileDescriptors();
        return -1;
    }




    while (1) {
        if (readBytes(buffer, 8) == 8) {
            char logString[128];
            memset(logString, 0, sizeof(logString));
            sendBytesToEcu(buffer, 8);
            convertToStringRepresentation(logString, buffer, 8);
            logMessage(logString);
        }
        sleep(1);
    }
}

void convertToStringRepresentation(char *dest, const char *source, unsigned int size) {
    char convertedValueToHexString[16];
    for (int i=0; i<8; ++i){
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
    if (writeRequest(ecuSocketFd, ForwardFacingRadarToCentralEcuRequester, bytes, nBytes) < 0) {
        logLastError();
        closeSocket(ecuSocketFd);
        return;
    }
    if (closeSocket(ecuSocketFd) < 0) logLastError();
}

int readBytes(char * buffer, unsigned int nBytes) {
    return (int)read(dataSourceFileFd, buffer, nBytes);
}


