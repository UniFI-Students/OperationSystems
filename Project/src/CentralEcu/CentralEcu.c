
#include <unistd.h>
#include <wait.h>
#include "CentralEcuIpc.h"
#include "../InterProcessComunication/Ipc.h"


int cEcuSocketFd;


void handleHmiRequest(const void *requestData, int requestDataLength);

int main()
{
    cEcuSocketFd = createUnixSocket(DEFAULT_PROTOCOL);
    cEcuSocketFd = bindUnixSocket(cEcuSocketFd, CENTRAL_ECU_SOCKET_NAME);
    listenSocket(cEcuSocketFd, 5);
    while(1)
    {
        int acceptedSocket = acceptUnixSocket(cEcuSocketFd);

        int forkedPId = fork();
        if (forkedPId == 0){
            int requesterId;
            void* requestData;
            int requestDataLength;

            readRequest(acceptedSocket, &requesterId, &requestData, &requestDataLength);

            switch((CEcuExternalCommunicator )requesterId){
                case HumanMachineInterface:
                    handleHmiRequest(requestData, requestDataLength);
                    break;
                case FrontWindShieldCamera:
                    break;
                case ForwardFacingRadar:
                    break;
                case ParkAssist:
                    break;
                default:
                    break;
            }
        }
        else wait(NULL);
    }
}

void handleHmiRequest(const void *requestData, int requestDataLength) {

}
