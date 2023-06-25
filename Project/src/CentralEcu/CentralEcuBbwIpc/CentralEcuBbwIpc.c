#include <signal.h>
#include <unistd.h>
#include "CentralEcuBbwIpc.h"
#include "../../Logger/Logger.h"
#include "../../BrakeByWire/BrakeByWireIpc.h"
#include "../../InterProcessComunication/Ipc.h"
#include "../../Shared/Consts.h"


void sendStopSignalToBbw(int brakeByWirePid) {
    kill(brakeByWirePid, SIG_STOP_CAR);
}

void sendBrakeRequestToBbw(int brakeQuantity) {
    BrakeByWireCommand cmd;
    cmd.type = Decrement;
    cmd.quantity = brakeQuantity;

    int bbwSocket = createInetSocket(DEFAULT_PROTOCOL);
    if (bbwSocket < 0) {
        logLastError();
        return;
    }
    int bbwConnectionRes = connectLocalInetSocket(bbwSocket, BRAKE_BY_WIRE_INET_SOCKET_PORT);
    while (bbwConnectionRes < 0) {
        logLastErrorWithWhenMessage("Trying to connect to bbw");
        bbwConnectionRes = connectLocalInetSocket(bbwSocket, BRAKE_BY_WIRE_INET_SOCKET_PORT);
        sleep(1);
    }

    if (writeRequest(bbwSocket, CentralEcuToBbwRequester, &cmd, sizeof(cmd))) {
        logLastError();
    }
    closeSocket(bbwSocket);
}