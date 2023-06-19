#include <unistd.h>
#include "ForwardFacingRadar.h"
#include "../Logger/Logger.h"

#define FORWARD_RADAR_DATA_SOURCE_FILE "/dev/urandom"
#define FORWARD_RADAR_LOGFILE "radar.log"


int readBytes(char buffer[8]);
void sendBytesToEcu(char buffer[8]);

int main(){
    char buffer[8];
    while(1)
    {

        if (readBytes(buffer) == 8)
        {
            sendBytesToEcu(buffer);
            logMessage(FORWARD_RADAR_LOGFILE, buffer);
        }
        sleep(1);
    }
}

void sendBytesToEcu(char buffer[8]) {
    //TODO: Implement sendBytesToEcu
}

int readBytes(char buffer[8]) {
    //TODO: Implement readBytes
    return 8;
}
