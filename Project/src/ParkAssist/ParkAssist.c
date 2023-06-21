#include <unistd.h>
#include "ParkAssist.h"
#include "../Logger/Logger.h"

#define PARK_ASSIST_LOGFILE "assist.log"

void receiveParkCommandFromEcu();

void receive8BytesFromSurroundViewCameras(char buffer[8]);

void sendBytesToParkAssist(char buffer[8]);

int readBytes(char buffer[8]);

int main(){
    char buffer[8];
    while(1)
    {
        receiveParkCommandFromEcu();
        receive8BytesFromSurroundViewCameras(buffer);
        sendBytesToParkAssist(buffer);
        for (int i=0; i<30; ++i)
        {
            if (readBytes(buffer) == 8)
            {
                sendBytesToParkAssist(buffer);
                logMessage(PARK_ASSIST_LOGFILE, buffer);
            }
            sleep(1);
        }
    }
}

int readBytes(char buffer[8]) {
    //TODO: Implement readBytes
    return 8;
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
