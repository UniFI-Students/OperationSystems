#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include "../Logger/Logger.h"
#include "../Shared/Consts.h"
#include "../FilePathProvider/FilePathProvider.h"
#include "../CentralEcu/CentralEcuIpc.h"
#include "../Shared/Utils.h"
#include "ForwardFacingRadar.h"


char dataSourceFilePath[128];
int dataSourceFileFd;

int readBytes(char *buffer, unsigned int nBytes);


void closeFileDescriptors();


void handleInterruptSignal();

void logBytes(const char *bytes, unsigned int nBytes);

void assignDataSourceFilePath(char *argStr);

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


    setLogFileName(FORWARD_RADAR_LOGFILE);
    setErrorLogFileName(FORWARD_RADAR_ERROR_LOGFILE);
    instantiateLogFileDescriptor();
    instantiateErrorLogFileDescriptor();

    dataSourceFileFd = open(dataSourceFilePath, O_RDONLY);
    if (dataSourceFileFd < 0) {
        logLastErrorWithWhenMessage("opening a ffr source file");
        closeFileDescriptors();
        return -1;
    }

    char buffer[8];
    while (1) {
        if (readBytes(buffer, 8) == 8) {
            sendDataToEcu(ForwardFacingRadarToCentralEcuRequester, buffer, 8);
            logBytes(buffer, 8);

        }
        sleep(1);
    }
}

void assignDataSourceFilePath(char *argStr) {
    if (strcmp(argStr, NORMAL_EXECUTION) == 0)
        strcpy(dataSourceFilePath, NORMAL_EXECUTION_RANDOM_DATASOURCE);
    if (strcmp(argStr, ARTIFICIAL_EXECUTION) == 0)
        getCwdWithFileName(ARTIFICIAL_EXECUTION_RANDOM_DATASOURCE, dataSourceFilePath, sizeof(dataSourceFilePath));
}

void logBytes(const char *bytes, unsigned int nBytes) {
    char logString[128];
    memset(logString, 0, sizeof(logString));
    convertBytesToStringRepresentation(logString, bytes, nBytes);
    logMessage(logString);
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


int readBytes(char *buffer, unsigned int nBytes) {
    return (int) read(dataSourceFileFd, buffer, nBytes);
}


