#include "CentralEcuIpc.h"
#include "../InterProcessComunication/Ipc.h"
#include "../Logger/Logger.h"
#include "../Shared/Consts.h"


int sendDataToEcu(CentralEcuRequester requester, const void *data, unsigned int dataLength){
    int ecuSocketFd = createInetSocket(DEFAULT_PROTOCOL);
    if (ecuSocketFd < 0) {
        logLastErrorWithWhenMessage("creating a socket to connect to the ecu");
        return -1;
    }
    if (connectLocalInetSocket(ecuSocketFd, CENTRAL_ECU_INET_SOCKET_PORT) < 0) {
        logLastErrorWithWhenMessage("connecting to the ecu");
        closeSocket(ecuSocketFd);
        return -1;
    }
    if (writeRequest(ecuSocketFd, requester, data, dataLength) < 0) {
        logLastErrorWithWhenMessage("writing a request to the ecu");
        closeSocket(ecuSocketFd);
        return -1;
    }
    closeSocket(ecuSocketFd);
    return 0;
}