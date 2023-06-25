#include <unistd.h>
#include "CentralEcuPaIpc.h"
#include "../../Logger/Logger.h"
#include "../../InterProcessComunication/Ipc.h"
#include "../../Shared/Consts.h"
#include "../../ParkAssist/ParkAssistIpc.h"

void activateParkAssist() {
    int paSocket = createInetSocket(DEFAULT_PROTOCOL);
    if (paSocket < 0) {
        logLastError();
        return;
    }
    int paConnectionRes = connectLocalInetSocket(paSocket, PARK_ASSIST_INET_SOCKET_PORT);
    while (paConnectionRes < 0) {
        logLastErrorWithWhenMessage("Trying to connect to pa.");
        paConnectionRes = connectLocalInetSocket(paSocket, PARK_ASSIST_INET_SOCKET_PORT);
        sleep(1);
    }

    if (writeRequest(paSocket, CentralEcuToParkAssistRequester, NULL, 0)) {
        logLastError();
    }
    closeSocket(paSocket);
}