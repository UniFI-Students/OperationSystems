#include <unistd.h>
#include "ForwardFacingRadar.h"
#include "../Logger/Logger.h"

#define FORWARD_RADAR_LOGFILE "radar.log"
#define FORWARD_RADAR_ERROR_LOGFILE "radar.eLog"


int readBytes(char buffer[8]);

void sendBytesToEcu(char buffer[8]);

int main() {
    char buffer[8];
    
    setLogFileName(FORWARD_RADAR_LOGFILE);
    setErrorLogFileName(FORWARD_RADAR_ERROR_LOGFILE);

    while (1) {

        if (readBytes(buffer) == 8) {
            sendBytesToEcu(buffer);
            logMessage(buffer);
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
