#include <unistd.h>
#include "SurroundViewCameras.h"
#include "../Logger/Logger.h"

#define SURROUND_CAMERAS_DATA_SOURCE_FILE "/dev/urandom"
#define SURROUND_CAMERAS_LOGFILE "cameras.log"

int readBytes(char buffer[8]);
void sendBytesToParkAssist(char buffer[8]);

int main()
{
    char buffer[8];
    while(1)
    {

        if (readBytes(buffer) == 8)
        {
            sendBytesToParkAssist(buffer);
            logMessage(SURROUND_CAMERAS_LOGFILE, buffer);
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