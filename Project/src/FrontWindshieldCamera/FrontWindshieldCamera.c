#include <unistd.h>
#include "FrontWindshieldCamera.h"
#include "../Logger/Logger.h"

#define FRONT_CAMERA_DATA_SOURCE_FILE "frontCamera.data"
#define FRONT_CAMERA_LOGFILE "camera.log"
#define FRONT_CAMERA_ERROR_LOGFILE "camera.eLog"

void readDataFromSourceFile(const char *sourceFilePath, char *data);
void sendDataToEcu(const char* message);

int main()
{
    char buff[64];

    setLogFileName(FRONT_CAMERA_LOGFILE);
    setErrorLogFileName(FRONT_CAMERA_ERROR_LOGFILE);

    while(1)
    {
        readDataFromSourceFile(FRONT_CAMERA_DATA_SOURCE_FILE, buff);
        sendDataToEcu(buff);
        logMessage(buff);
        sleep(1);
    }
}

void sendDataToEcu(const char* message) {
    //TODO: Implement sendDataToEcu
}

void readDataFromSourceFile(const char *sourceFilePath, char *data)
{
    //TODO: Implement readDataFromSourceFile
}
