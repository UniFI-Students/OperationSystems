#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "SurroundViewCameras.h"
#include "../Logger/Logger.h"
#include "../Shared/Consts.h"
#include "../FilePathProvider/FilePathProvider.h"


#define SURROUND_CAMERAS_LOGFILE "cameras.log"
#define SURROUND_CAMERAS_ERROR_LOGFILE "cameras.ELog"

char dataSourceFilePath[128];
int dataSourceFileFd;

int readBytes(char *buffer, unsigned int nBytes);

void sendBytesToParkAssist(char buffer[8]);

void closeFileDescriptors();

int main(int argc, char *argv[]) {
    char buffer[8];

    setLogFileName(SURROUND_CAMERAS_LOGFILE);
    setErrorLogFileName(SURROUND_CAMERAS_ERROR_LOGFILE);
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
    if (dataSourceFileFd < 0) {
        logMessage(dataSourceFilePath);
        logLastErrorWithMessage("Data source file not found.");
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
            sendBytesToParkAssist(buffer);
            logMessage(buffer);
        }
        sleep(1);
    }
}

void closeFileDescriptors() {
    close(dataSourceFileFd);
    closeLogFileDescriptor();
    closeErrorLogFileDescriptor();

}

void sendBytesToParkAssist(char buffer[8]) {
    //TODO: Implement sendBytesToEcu
}

int readBytes(char *buffer, unsigned int nBytes) {
    return (int) read(dataSourceFileFd, buffer, nBytes);
}