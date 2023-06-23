
#include <unistd.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CentralEcuIpc.h"
#include "CentralEcuBbwIpc/CentralEcuBbwIpc.h"
#include "CentralEcuHmiIpc/CentralEcuHmiIpc.h"
#include "CentralEcuSbwIpc/CentralEcuSbwIpc.h"
#include "CentralEcuTcIpc/CentralEcuTcIpc.h"
#include "../Logger/Logger.h"
#include "../InterProcessComunication/Ipc.h"
#include "../HumanMachineInterface/HumanMachineInterfaceIpc.h"
#include "../Shared/Consts.h"

#define CENTRAL_ECU_LOGFILE "ECU.log"
#define CENTRAL_ECU_ERROR_LOGFILE "ECU.eLog"

enum CarState {
    CarStateNone,
    CarStateStarted,
    CarStateParking
} typedef CarState;

int cEcuSocketFd;
int acceptedSocket;


int steerByWirePid;
int throttleControlPid;
int brakeByWirePid;

int frontWindShieldCameraPid;
int forwardFacingRadarPid;

int parkAssistPid;
int surroundViewCamerasPid;

int velocity;
CarState carState;



void registerSignalHandlers();
void handleInterruptSignal();

void handleHmiRequest(const void *requestDataPtr, unsigned int requestDataLength);

void handlePaRequest(void *requestDataPtr, unsigned int requestDataLength);

void handleFwcRequest(void *requestDataPtr, unsigned int requestDataLength);

void handleFfrRequest(void *requestDataPtr, unsigned int requestDataLength);




void handleStartCommandFromHmi();

void handleParkingCommandFromHmi();

void handleStopCommandFromHmi();

void closeFileDescriptors();

void runActuators();

void runSensors();

void runParkingSensors();


void initiateParking();

void getCwdWithFileName(const char *fileName, char* buff, int size);

void execEcuChildProcess(const char *childName);
void execEcuChildProcessWithIntArgument(const char *childName, int arg);



int main() {
    cEcuSocketFd = createInetSocket(DEFAULT_PROTOCOL);
    velocity = 0;
    carState = CarStateNone;

    setLogFileName(CENTRAL_ECU_LOGFILE);
    setErrorLogFileName(CENTRAL_ECU_ERROR_LOGFILE);
    instantiateLogFileDescriptor();
    instantiateErrorLogFileDescriptor();

    if (cEcuSocketFd < 0) {
        logLastError();
        closeFileDescriptors();
        exit(-1);
    }
    if (bindLocalInetSocket(cEcuSocketFd, CENTRAL_ECU_INET_SOCKET_PORT) < 0) {
        logLastError();
        closeFileDescriptors();
        exit(-1);
    }
    if (listenSocket(cEcuSocketFd, 5) < 0) {
        logLastError();
        closeFileDescriptors();
        exit(-1);
    }




    registerSignalHandlers();

    while (1) {
        acceptedSocket = acceptInetSocket(cEcuSocketFd);
        if (acceptedSocket < 0) {
            logLastError();
            closeFileDescriptors();
            exit(-1);
        }

        int requesterId;
        void *requestDataPtr;
        unsigned int requestDataLength;

        if (readRequest(acceptedSocket, &requesterId, &requestDataPtr, &requestDataLength) < 0) logLastError();

        switch ((CentralEcuRequester) requesterId) {
            case HumanMachineInterfaceToCentralEcuRequester:
                handleHmiRequest(requestDataPtr, requestDataLength);
                break;
            case FrontWindShieldCameraToCentralEcuRequester:
                handleFwcRequest(requestDataPtr, requestDataLength);
                break;
            case ForwardFacingRadarToCentralEcuRequester:
                handleFfrRequest(requestDataPtr, requestDataLength);
                break;
            case ParkAssistToCentralEcuRequester:
                handlePaRequest(requestDataPtr, requestDataLength);
                break;
            default:
                logLastErrorWithMessage("Unknown request arrived.");
                break;
        }
        free(requestDataPtr);

        if (closeSocket(acceptedSocket) < 0) {
            logLastError();
            closeFileDescriptors();
            exit(-1);
        }
    }

}

void closeFileDescriptors() {
    closeLogFileDescriptor();
    closeErrorLogFileDescriptor();
    closeSocket(cEcuSocketFd);
    closeSocket(acceptedSocket);
}


void registerSignalHandlers() {
    signal(SIGINT, handleInterruptSignal);
    signal(SIGCHLD, SIG_IGN);
}

void handleInterruptSignal() {
    closeFileDescriptors();
    exit(0);
}

void handleHmiRequest(const void *requestDataPtr, unsigned int requestDataLength) {
    HumanMachineInterfaceCommand *cmdPtr = (HumanMachineInterfaceCommand *) requestDataPtr;
    switch (cmdPtr->type) {

        case Start:
            handleStartCommandFromHmi();
            break;
        case Parking:
            handleParkingCommandFromHmi();
            break;
        case Stop:
            handleStopCommandFromHmi();
            break;
        case None:
            logLastErrorWithMessage("Invalid hmi command type");
            break;
    }

}

void handleStartCommandFromHmi() {
    if (carState != CarStateNone) return;
    carState = CarStateStarted;
    runActuators();
    runSensors();
}


void runParkingSensors() {
    parkAssistPid = fork();
    if (parkAssistPid == 0) execEcuChildProcess(PARK_ASSIST_EXE_FILENAME);
    surroundViewCamerasPid = fork();
    if (surroundViewCamerasPid == 0) execEcuChildProcess(SURROUND_VIEW_CAMERAS_EXE_FILENAME);
}

void stopParkingSensors() {
    if (parkAssistPid != 0) kill(parkAssistPid, SIGINT);
    if (surroundViewCamerasPid != 0) kill(surroundViewCamerasPid, SIGINT);
    parkAssistPid = 0;
    surroundViewCamerasPid = 0;
}


void runSensors() {
    frontWindShieldCameraPid = fork();
    if (frontWindShieldCameraPid == 0) execEcuChildProcess(FRONT_WIND_SHIELD_CAMERA_EXE_FILENAME);

    forwardFacingRadarPid = fork();
    if (forwardFacingRadarPid == 0) execEcuChildProcess(FORWARD_FACING_RADAR_EXE_FILENAME);
}



void stopSensors(){
    if (frontWindShieldCameraPid != 0) kill(frontWindShieldCameraPid, SIGINT);
    if (forwardFacingRadarPid != 0) kill(forwardFacingRadarPid, SIGINT);
    frontWindShieldCameraPid = 0;
    forwardFacingRadarPid = 0;
}

void runActuators() {
    int cEcuPid = getpid();
    steerByWirePid = fork();
    if (steerByWirePid == 0) execEcuChildProcess(STEER_BY_WIRE_EXE_FILENAME);
    throttleControlPid = fork();
    if (throttleControlPid == 0) execEcuChildProcessWithIntArgument(THROTTLE_CONTROL_EXE_FILENAME, cEcuPid);
    brakeByWirePid = fork();
    if (brakeByWirePid == 0) execEcuChildProcess(BRAKE_BY_WIRE_EXE_FILENAME);
}

void getCwdWithFileName(const char *fileName, char* buff, int size){
    getcwd(buff, size);
    strcat(buff, "/");
    strcat(buff, fileName);
}

void stopActuators(){
    if (steerByWirePid != 0) kill(steerByWirePid, SIGINT);
    if (throttleControlPid != 0) kill(throttleControlPid, SIGINT);
    if (brakeByWirePid != 0) kill(brakeByWirePid, SIGINT);
    steerByWirePid = 0;
    throttleControlPid = 0;
    brakeByWirePid = 0;
}

void execEcuChildProcess(const char *childName) {
    closeFileDescriptors();
    char buff[128];
    getCwdWithFileName(childName, buff, sizeof(buff));
    execl(buff, childName, NULL);
    logLastError();
    exit(-1);
}

void execEcuChildProcessWithIntArgument(const char *childName, int arg) {
    closeFileDescriptors();
    char buff[128];
    char argStr[16];
    sprintf(argStr, "%d", arg);
    getCwdWithFileName(childName, buff, sizeof(buff));
    execl(buff, childName, argStr, (char*)0);
    logLastError();
    exit(-1);
}

void handleParkingCommandFromHmi() {
    initiateParking();

}

void initiateParking() {
    //TODO: Implement initiateParking
    carState = CarStateParking;
}

void handleStopCommandFromHmi() {
    velocity = 0;
    sendStopSignalToBbw(brakeByWirePid);
}








void handleFwcRequest(void *requestDataPtr, unsigned int requestDataLength) {
    if (carState != CarStateStarted) return;
    printf("FWSC REQUEST\n");
}

void handleFfrRequest(void *requestDataPtr, unsigned int requestDataLength) {
    if (carState != CarStateStarted) return;
    printf("FFR REQUEST\n");
}




void handlePaRequest(void *requestDataPtr, unsigned int requestDataLength) {
    if (!carState) return;
    printf("Pa REQUEST\n");
}
