#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "ParkAssist.h"
#include "../Logger/Logger.h"
#include "../FilePathProvider/FilePathProvider.h"
#include "../Shared/Consts.h"

#define PARK_ASSIST_LOGFILE "assist.log"
#define PARK_ASSIST_ERROR_LOGFILE "assist.eLog"

char dataSourceFilePath[128];
int dataSourceFileFd;

void receiveParkCommandFromEcu();

void receive8BytesFromSurroundViewCameras(char buffer[8]);

void sendBytesToParkAssist(char buffer[8]);

int readBytes(char * buffer, unsigned int nBytes);

void closeFileDescriptors();

int main(int argc, char* argv[]){
    char buffer[8];

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

    dataSourceFileFd = open(dataSourceFilePath, O_RDONLY);
    if (dataSourceFileFd < 0){
        logMessage(dataSourceFilePath);
        logLastErrorWithMessage("Data source file not found.");
        closeFileDescriptors();
        return -1;
    }

    setLogFileName(PARK_ASSIST_LOGFILE);
    setErrorLogFileName(PARK_ASSIST_ERROR_LOGFILE);

    while(1)
    {
        receiveParkCommandFromEcu();
        receive8BytesFromSurroundViewCameras(buffer);
        sendBytesToParkAssist(buffer);
        for (int i=0; i<30; ++i)
        {
            if (readBytes(buffer, 8) == 8)
            {
                sendBytesToParkAssist(buffer);
                logMessage(buffer);
            }
            sleep(1);
        }
    }
}

void closeFileDescriptors() {
    close(dataSourceFileFd);
    closeLogFileDescriptor();
    closeErrorLogFileDescriptor();

}

int readBytes(char * buffer, unsigned int nBytes) {
    return (int)read(dataSourceFileFd, buffer, nBytes);
}

void sendBytesToParkAssist(char buffer[8]) {

    //TODO: Implement sendBytesToParkAssist
}

void receive8BytesFromSurroundViewCameras(char buffer[8]) {

    //TODO: Implement receive8BytesFromSurroundViewCameras
}

void receiveParkCommandFromEcu() {

    //TODO: Implement receiveParkCommandFromEcu
}
