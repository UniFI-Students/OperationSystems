#include <unistd.h>
#include "CentralEcuSbwIpc.h"
#include "../../InterProcessComunication/Ipc.h"
#include "../../Logger/Logger.h"
#include "../../Shared/Consts.h"

void sendSteerRequestToSbw(SteerByWireCommandType type){
    SteerByWireCommand cmd;
    cmd.type = type;

    int swbSocket = createInetSocket(DEFAULT_PROTOCOL);
    if (swbSocket < 0) {
        logLastError();
        return;
    }
    int sbwConnectionRes = connectLocalInetSocket(swbSocket, STEER_BY_WIRE_INET_SOCKET_PORT);
    while (sbwConnectionRes < 0) {
        logLastErrorWithWhenMessage("Trying to connect to sbw");
        sbwConnectionRes = connectLocalInetSocket(swbSocket, STEER_BY_WIRE_INET_SOCKET_PORT);
        sleep(1);
    }

    if (writeRequest(swbSocket, CentralEcuToSbwRequester, &cmd, sizeof(cmd))) {
        logLastError();
    }
    closeSocket(swbSocket);
}
