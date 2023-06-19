#include <unistd.h>
#include "FrontWindshieldCamera.h"
#include "../Logger/Logger.h"

#define FRONT_CAMERA_DATA_SOURCE_FILE "frontCamera.data"
#define FRONT_CAMERA_LOGFILE "camera.log"

void readDataFromSourceFile(const char *sourceFilePath, char *data);
void sendDataToEcu(const char* message);

int main()
{
    char buff[64];

    while(1)
    {
        readDataFromSourceFile(FRONT_CAMERA_DATA_SOURCE_FILE, buff);
        sendDataToEcu(buff);
        logMessage(FRONT_CAMERA_LOGFILE, buff);
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
