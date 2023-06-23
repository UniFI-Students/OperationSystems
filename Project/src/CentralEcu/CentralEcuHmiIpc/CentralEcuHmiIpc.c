#include <unistd.h>
#include <string.h>
#include "CentralEcuHmiIpc.h"
#include "../../InterProcessComunication/Ipc.h"
#include "../../Logger/Logger.h"
#include "../../HumanMachineInterface/HumanMachineInterfaceIpc.h"
#include "../../Shared/Consts.h"

void sendMessageToHmi(const char *message) {
    int hmiSocketFd = createInetSocket(DEFAULT_PROTOCOL);
    if (hmiSocketFd < 0) {
        logLastError();
        return;
    }
    int hmiConnectionRes = connectLocalInetSocket(hmiSocketFd, HUMAN_MACHINE_INTERFACE_INET_SOCKET_PORT);
    while (hmiConnectionRes < 0) {
        logLastErrorWithMessage("Trying to connect to hmi");
        hmiConnectionRes = connectLocalInetSocket(hmiSocketFd, HUMAN_MACHINE_INTERFACE_INET_SOCKET_PORT);
        sleep(1);
    }

    if (writeRequest(hmiSocketFd, CentralEcuToHmiRequester, message, strlen(message)) < 0) {
        logLastError();
    }
    closeSocket(hmiSocketFd);
}
