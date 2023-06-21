#include <unistd.h>
#include "SurroundViewCameras.h"
#include "../Logger/Logger.h"

#define SURROUND_CAMERAS_DATA_SOURCE_FILE "/dev/urandom"

#define SURROUND_CAMERAS_LOGFILE "cameras.log"
#define SURROUND_CAMERAS_ERROR_LOGFILE "cameras.ELog"

int readBytes(char buffer[8]);
void sendBytesToParkAssist(char buffer[8]);

int main()
{
    char buffer[8];

    setLogFileName(SURROUND_CAMERAS_LOGFILE);
    setErrorLogFileName(SURROUND_CAMERAS_ERROR_LOGFILE);

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