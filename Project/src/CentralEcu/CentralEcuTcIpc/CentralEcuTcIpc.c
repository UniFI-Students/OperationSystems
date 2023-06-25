#include <unistd.h>
#include "CentralEcuTcIpc.h"
#include "../../ThrottleControl/ThrottleControlIpc.h"
#include "../../InterProcessComunication/Ipc.h"
#include "../../Logger/Logger.h"
#include "../../Shared/Consts.h"

void sendThrottleRequestToTc(int throttleQuantity){
    ThrottleControlCommand cmd;
    cmd.type = Increment;
    cmd.quantity = throttleQuantity;

    int tcSocket = createInetSocket(DEFAULT_PROTOCOL);
    if (tcSocket < 0) {
        logLastError();
        return;
    }
    int tcConnectionRes = connectLocalInetSocket(tcSocket, THROTTLE_CONTROL_INET_SOCKET_PORT);
    while (tcConnectionRes < 0) {
        logLastErrorWithWhenMessage("Trying to connect to tc");
        tcConnectionRes = connectLocalInetSocket(tcSocket, THROTTLE_CONTROL_INET_SOCKET_PORT);
        sleep(1);
    }

    if (writeRequest(tcSocket, CentralEcuToTcRequester, &cmd, sizeof(cmd))) {
        logLastError();
    }
    closeSocket(tcSocket);

}