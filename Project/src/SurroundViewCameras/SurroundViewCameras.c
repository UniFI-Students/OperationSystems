#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "SurroundViewCameras.h"
#include "../Logger/Logger.h"


#define SURROUND_CAMERAS_LOGFILE "cameras.log"
#define SURROUND_CAMERAS_ERROR_LOGFILE "cameras.ELog"

char dataSourceFilePath[128];

int readBytes(char buffer[8]);
void sendBytesToParkAssist(char buffer[8]);

int main(int argc, char* argv[])
{
    char buffer[8];

    setLogFileName(SURROUND_CAMERAS_LOGFILE);
    setErrorLogFileName(SURROUND_CAMERAS_ERROR_LOGFILE);
    instantiateLogFileDescriptor();
    instantiateErrorLogFileDescriptor();


    if (argc <= 1){
        logLastErrorWithMessage("Unassigned argument for execution type.");
        exit(-1);
    }

    strcpy(dataSourceFilePath, argv[1]);

    while(1)
    {

        if (readBytes(buffer) == 8)
        {
            sendBytesToParkAssist(buffer);
            logMessage(buffer);
        }
        sleep(1);
    }
}

void sendBytesToParkAssist(char buffer[8]) {
    //TODO: Implement sendBytesToEcu
}

int readBytes(char buffer[8]) {
    //TODO: Implement readBytes
    return 8;
}