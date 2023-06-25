#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include "../Logger/Logger.h"
#include "../CentralEcu/CentralEcuIpc.h"
#include "../FilePathProvider/FilePathProvider.h"
#include "../Shared/Consts.h"
#include "FrontWindshieldCamera.h"


FILE *dataSourceFilePtr;
char dataSourceFilePath[128];

void readDataFromSourceFile(char *dataBuff);

void closeFileDescriptors();

void handleInterruptSignal();

int main() {
    signal(SIGINT, handleInterruptSignal);
    getCwdWithFileName(FRONT_CAMERA_DATA_SOURCE_FILE, dataSourceFilePath, sizeof(dataSourceFilePath));

    setLogFileName(FRONT_CAMERA_LOGFILE);
    setErrorLogFileName(FRONT_CAMERA_ERROR_LOGFILE);

    instantiateLogFileDescriptor();
    instantiateErrorLogFileDescriptor();
    dataSourceFilePtr = fopen(dataSourceFilePath, "r");

    char buff[64];
    while (!feof(dataSourceFilePtr)) {
        readDataFromSourceFile(buff);
        sendDataToEcu(FrontWindShieldCameraToCentralEcuRequester, buff, strlen(buff));
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

void readDataFromSourceFile(char *dataBuff) {
    fscanf(dataSourceFilePtr, "%s\n", dataBuff);
}
