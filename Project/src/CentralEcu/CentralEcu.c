
#include <unistd.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CentralEcuIpc.h"
#include "../Logger/Logger.h"
#include "../InterProcessComunication/Ipc.h"
#include "../HumanMachineInterface/HumanMachineInterfaceIpc.h"

#define CENTRAL_ECU_LOGFILE "ECU.log"
#define CENTRAL_ECU_ERROR_LOGFILE "ECU.eLog"

enum CarState {
    CarStateNone,
    CarStateStarted,
    CarStateParking
} typedef CarState;

int cEcuSocketFd;


int steerByWirePid;
int throttleControlPid;
int brakeByWirePid;

int frontWindShieldCameraPid;
int forwardFacingRadarPid;

int parkAssistPid;
int surroundViewCamerasPid;

int velocity;
CarState carState;


void handleInterruptSignal();

void handleHmiRequest(const void *requestData, int requestDataLength);

void registerSignalHandlers();

void handlePaRequest(void *pVoid, int length);

void handleFwscRequest(void *pVoid, int length);

void handleFfrRequest(void *pVoid, int length);

void handleStartCommandFromHmi();

void handleParkingCommandFromHmi();

void handleStopCommandFromHmi();

void sendMessageToHmi(char *message);

void closeFileDescriptors();

void runActuators();

void runSensors();

void runParkingSensors();

void sendStopSignalToBreakByWire();

void initiateParking();

void getCwdWithFileName(const char *fileName, char* buff, int size);

void execEcuChildProcess(const char *childName);

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
        int acceptedSocket = acceptInetSocket(cEcuSocketFd);
        if (acceptedSocket < 0) {
            logLastError();
            closeFileDescriptors();
            exit(-1);
        }

        int requesterId;
        void *requestData;
        int requestDataLength;

        if (readRequest(acceptedSocket, &requesterId, &requestData, &requestDataLength) < 0) logLastError();

        switch ((CentralEcuRequester) requesterId) {
            case HumanMachineInterfaceToCentralEcuRequester:
                handleHmiRequest(requestData, requestDataLength);
                break;
            case FrontWindShieldCameraToCentralEcuRequester:
                handleFwscRequest(requestData, requestDataLength);
                break;
            case ForwardFacingRadarToCentralEcuRequester:
                handleFfrRequest(requestData, requestDataLength);
                break;
            case ParkAssistToCentralEcuRequester:
                handlePaRequest(requestData, requestDataLength);
                break;
            default:
                logLastErrorWithMessage("Unknown request arrived.");
                break;
        }
        free(requestData);
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
}


void registerSignalHandlers() {
    signal(SIGINT, handleInterruptSignal);
    signal(SIGCHLD, SIG_IGN);
}

void handleInterruptSignal() {
    closeFileDescriptors();
    exit(0);
}

void handleHmiRequest(const void *requestData, int requestDataLength) {
    HumanMachineInterfaceCommand *cmdPtr = (HumanMachineInterfaceCommand *) requestData;
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

void execEcuChildProcess(const char *childName) {
    closeFileDescriptors();
    char buff[128];
    getCwdWithFileName(childName, buff, sizeof(buff));
    execl(buff, childName, NULL);
    logLastError();
    exit(-1);
}

void stopSensors(){
    if (frontWindShieldCameraPid != 0) kill(frontWindShieldCameraPid, SIGINT);
    if (forwardFacingRadarPid != 0) kill(forwardFacingRadarPid, SIGINT);
    frontWindShieldCameraPid = 0;
    forwardFacingRadarPid = 0;
}

void runActuators() {
    steerByWirePid = fork();
    if (steerByWirePid == 0) execEcuChildProcess(STEER_BY_WIRE_EXE_FILENAME);
    throttleControlPid = fork();
    if (throttleControlPid == 0) execEcuChildProcess(THROTTLE_CONTROL_EXE_FILENAME);
    brakeByWirePid = fork();
    if (brakeByWirePid == 0) execEcuChildProcess(BREAK_BY_WIRE_EXE_FILENAME);
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

void handleParkingCommandFromHmi() {
    initiateParking();

}

void initiateParking() {
    //TODO: Implement initiateParking
    carState = CarStateParking;
}

void handleStopCommandFromHmi() {
    sendStopSignalToBreakByWire();
    velocity = 0;
}

void sendStopSignalToBreakByWire() {
    logMessage("Sending stop signal to brake by wire.");
    sendMessageToHmi("Sending stop signal to brake by wire.");
    kill(brakeByWirePid, SIGUSR1);
}

void sendMessageToHmi(char *message) {

    int hmiSocketFd = createInetSocket(DEFAULT_PROTOCOL);
    if (hmiSocketFd < 0) {
        logLastError();
        closeFileDescriptors();
        exit(-1);
    }
    int hmiConnectionRes = connectLocalInetSocket(hmiSocketFd, HUMAN_MACHINE_INTERFACE_INET_SOCKET_PORT);
    while (hmiConnectionRes < 0) {
        logLastErrorWithMessage("Trying to connect to hmi");
        hmiConnectionRes = connectLocalInetSocket(hmiSocketFd, HUMAN_MACHINE_INTERFACE_INET_SOCKET_PORT);
        sleep(5);
    }

    if (writeRequest(hmiSocketFd, CentralEcuToHmiRequester, message, strlen(message)) < 0) {
        logLastError();
    }
    closeSocket(hmiSocketFd);
}

void handleFwscRequest(void *pVoid, int length) {
    if (carState != CarStateStarted) return;
    printf("FWSC REQUEST\n");
}

void handleFfrRequest(void *pVoid, int length) {
    if (carState != CarStateStarted) return;
    printf("FFR REQUEST\n");
}


void handlePaRequest(void *pVoid, int length) {
    if (!carState) return;
    printf("Pa REQUEST\n");
}
